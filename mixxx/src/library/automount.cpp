#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QRegExp>
#include <QTime>

#include "automount.h"

Automount::Automount(QObject* parent)
         : QObject(parent),
           m_timer(this) {
    connect(&m_timer, SIGNAL(timeout()), this , SLOT(slotReadMtab()));
    m_timer.start(5000);// call every 5 seconds

    // generated list of devices known at start
    m_devices = attachedDevices();
}

Automount::~Automount(){
    
}

void Automount::slotReadMtab(){
    // QTime time;
    // time.start();

    // look for connected devices
    QStringList devices = attachedDevices();

    if (m_devices.size()!=devices.size()) {
        if (m_devices.size() > devices.size()) {
            removedDevice(devices);
        } else {
            addedDevice(devices);
        }
    }
    // qDebug("checking for changes took: %d ms" , time.elapsed());
}

QStringList Automount::attachedDevices(){
    // This will parse the entries in /etc/mtab to look for known storage devices
    // a line in this file with a device will typically look like this.
    //
    // /dev/sda1 /home ext4 rw,relatime,user_xattr,acl,barrier=1,data=ordered 0 0
    //
    // we want to extract /home here. So we cut of everything up to the first 
    // space. The word after the location is the fs used on that device. For this
    // we look with a Regex for the first apperiance of the name of a fs.

    // QTime time;
    // time.start();
    // get supported files systems
    QStringList fs;
    QFile filesystems("/proc/filesystems");
    if (filesystems.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString line = filesystems.readLine();
        while (!line.isEmpty()) {
            if (!line.contains("nodev")) {
                fs << line.trimmed();
            }
            line = filesystems.readLine();
        }
    }
    filesystems.close();

    QFile file("/etc/mtab");
    QStringList devices;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QString line = file.readLine();
        while (!line.isEmpty()) {
            if (line.contains("/dev/sd")) {
                line = line.right(line.size() - line.indexOf(" "));
                line = line.left(line.indexOf(QRegExp(fs.join("|"))));
                devices.append(line.trimmed());
            }
            line = file.readLine();
        }
    }
    file.close();
    // qDebug("reading file took: %d ms" , time.elapsed());
    return devices;
}

void Automount::removedDevice(QStringList devices){
    QStringList diff;
    foreach (QString device, m_devices) {
        if (!devices.removeOne(device)) {
            diff.append(device);
        }
    }
    qDebug() << "removed the following devices";
    qDebug() << diff;

    emit removedStorage(diff);
    foreach (QString device, diff) {
        m_devices.removeOne(device);
    }
}

void Automount::addedDevice(QStringList devices){
    QStringList diff;
    foreach (QString device, devices) {
        if (!m_devices.removeOne(device)) {
            diff.append(device);
        }
    }
    qDebug() << "added the following devices";
    qDebug() << diff;

    emit foundNewStorage(diff);
    m_devices = devices;
}
