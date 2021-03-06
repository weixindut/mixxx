/**
* @file portmidienumerator.cpp
* @author Sean Pappalardo spappalardo@mixxx.org
* @date Thu 15 Mar 2012
* @brief This class handles discovery and enumeration of DJ controller devices that appear under the PortMIDI cross-platform API.
*/

#include <portmidi.h>
#include <QRegExp>

#include "controllers/midi/portmidienumerator.h"

#include "controllers/midi/portmidicontroller.h"
#include "util/cmdlineargs.h"

bool shouldBlacklistDevice(const PmDeviceInfo* device) {
    QString deviceName = device->name;
    // In developer mode we show the MIDI Through Port, otherwise blacklist it
    // since it routinely causes trouble.
    return !CmdlineArgs::Instance().getDeveloper() &&
            deviceName.startsWith("Midi Through Port", Qt::CaseInsensitive);
}

PortMidiEnumerator::PortMidiEnumerator() : MidiEnumerator() {
}

PortMidiEnumerator::~PortMidiEnumerator() {
    qDebug() << "Deleting PortMIDI devices...";
    QListIterator<Controller*> dev_it(m_devices);
    while (dev_it.hasNext()) {
        delete dev_it.next();
    }
}

bool namesMatchMidiPattern(const QString input_name,
                           const QString output_name) {
    // Some platforms format MIDI device names as "deviceName MIDI ###" where
    // ### is the instance # of the device. Therefore we want to link two
    // devices that have an equivalent "deviceName" and ### section.
    QRegExp deviceNamePattern("^(.*) MIDI (\\d+)( .*)?$");

    int inputMatch = deviceNamePattern.indexIn(input_name);
    if (inputMatch == 0) {
        QString inputDeviceName = deviceNamePattern.cap(1);
        QString inputDeviceIndex = deviceNamePattern.cap(2);
        int outputMatch = deviceNamePattern.indexIn(output_name);
        if (outputMatch == 0) {
            QString outputDeviceName = deviceNamePattern.cap(1);
            QString outputDeviceIndex = deviceNamePattern.cap(2);
            if (outputDeviceName.compare(inputDeviceName, Qt::CaseInsensitive) == 0 &&
                outputDeviceIndex == inputDeviceIndex) {
                return true;
            }
        }
    }
    return false;
}

bool namesMatchPattern(const QString input_name,
                       const QString output_name) {
    // This is a broad pattern that matches a text blob followed by a numeral
    // potentially followed by non-numeric text. The non-numeric requirement is
    // meant to avoid corenr cases around devices with names like "Hercules RMX
    // 2" where we would potentially confuse the number in the device name as
    // the ordinal index of the device.
    QRegExp deviceNamePattern("^(.*) (\\d+)( [^0-9]+)?$");

    int inputMatch = deviceNamePattern.indexIn(input_name);
    if (inputMatch == 0) {
        QString inputDeviceName = deviceNamePattern.cap(1);
        QString inputDeviceIndex = deviceNamePattern.cap(2);
        int outputMatch = deviceNamePattern.indexIn(output_name);
        if (outputMatch == 0) {
            QString outputDeviceName = deviceNamePattern.cap(1);
            QString outputDeviceIndex = deviceNamePattern.cap(2);
            if (outputDeviceName.compare(inputDeviceName, Qt::CaseInsensitive) == 0 &&
                outputDeviceIndex == inputDeviceIndex) {
                return true;
            }
        }
    }
    return false;
}

bool shouldLinkInputToOutput(const QString input_name,
                             const QString output_name) {
    // Early exit.
    if (input_name == output_name) {
        return true;
    }

    // Some device drivers prepend "To" and "From" to the names of their MIDI
    // ports. If the output and input device names don't match, let's try
    // trimming those words from the start, and seeing if they then match.

    // Ignore "From" text in the device input name.
    QString input_name_stripped = input_name;
    if (input_name.indexOf("from", 0, Qt::CaseInsensitive) != -1) {
        input_name_stripped = input_name.right(input_name.length() - 4);
    }

    // Ignore "To" text in the device output name.
    QString output_name_stripped = output_name;
    if (output_name.indexOf("to", 0, Qt::CaseInsensitive) != -1) {
        output_name_stripped = output_name.right(output_name.length() - 2);
    }

    if (input_name_stripped == output_name_stripped ||
        namesMatchMidiPattern(input_name_stripped, output_name_stripped) ||
        namesMatchMidiPattern(input_name, output_name) ||
        namesMatchPattern(input_name_stripped, output_name_stripped) ||
        namesMatchPattern(input_name, output_name)) {
        return true;
    }
    return false;
}

/** Enumerate the MIDI devices
  * This method needs a bit of intelligence because PortMidi (and the underlying MIDI APIs) like to split
  * output and input into separate devices. Eg. PortMidi would tell us the Hercules is two half-duplex devices.
  * To help simplify a lot of code, we're going to aggregate these two streams into a single full-duplex device.
  */
QList<Controller*> PortMidiEnumerator::queryDevices() {
    qDebug() << "Scanning PortMIDI devices:";

    int iNumDevices = Pm_CountDevices();

    QListIterator<Controller*> dev_it(m_devices);
    while (dev_it.hasNext()) {
        delete dev_it.next();
    }

    m_devices.clear();

    const PmDeviceInfo* inputDeviceInfo = NULL;
    const PmDeviceInfo* outputDeviceInfo = NULL;
    int inputDevIndex = -1;
    int outputDevIndex = -1;
    QMap<int, QString> unassignedOutputDevices;

    // Build a complete list of output devices for later pairing
    for (int i = 0; i < iNumDevices; i++) {
        const PmDeviceInfo* deviceInfo = Pm_GetDeviceInfo(i);
        if (shouldBlacklistDevice(deviceInfo)) {
            continue;
        }
        if (deviceInfo->output) {
            qDebug() << " Found output device" << "#" << i << deviceInfo->name;
            QString deviceName = deviceInfo->name;
            unassignedOutputDevices[i] = deviceName;
        }
    }

    // Search for input devices and pair them with output devices if applicable
    for (int i = 0; i < iNumDevices; i++) {
        const PmDeviceInfo* deviceInfo = Pm_GetDeviceInfo(i);
        if (shouldBlacklistDevice(deviceInfo)) {
            continue;
        }

        //If we found an input device
        if (deviceInfo->input) {
            qDebug() << " Found input device" << "#" << i << deviceInfo->name;
            inputDeviceInfo = deviceInfo;
            inputDevIndex = i;

            //Reset our output device variables before we look for one incase we find none.
            outputDeviceInfo = NULL;
            outputDevIndex = -1;

            //Search for a corresponding output device
            QMapIterator<int, QString> j(unassignedOutputDevices);
            while (j.hasNext()) {
                j.next();

                QString deviceName = inputDeviceInfo->name;
                QString outputName = QString(j.value());

                if (shouldLinkInputToOutput(deviceName, outputName)) {
                    outputDevIndex = j.key();
                    outputDeviceInfo = Pm_GetDeviceInfo(outputDevIndex);

                    unassignedOutputDevices.remove(outputDevIndex);

                    qDebug() << "    Linking to output device #" << outputDevIndex << outputName;
                    break;
                }
            }

            // So at this point, we either have an input-only MIDI device
            // (outputDeviceInfo == NULL) or we've found a matching output MIDI
            // device (outputDeviceInfo != NULL).

            //.... so create our (aggregate) MIDI device!
            PortMidiController *currentDevice = new PortMidiController(
                inputDeviceInfo, outputDeviceInfo,
                inputDevIndex, outputDevIndex);
            m_devices.push_back(currentDevice);
        }

        // Is there a use-case for output-only devices (such as message
        // displays?) If so, handle them here.

        //else if (deviceInfo->output) {
        //    PortMidiController *currentDevice = new PortMidiController(deviceInfo, i);
        //    m_devices.push_back((MidiController*)currentDevice);
        //}
    }
    return m_devices;
}
