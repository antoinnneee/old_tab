/**
  @file
  @author Stefan Frings
  Pusblished for free use without any warranties.
*/

#ifndef ANDROIDRFCOMM_H
#define ANDROIDRFCOMM_H

#include <QMap>
#include <QString>
#include <QByteArray>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>

/**
 * RFCOMM connector for Android.
 * This class allows you to communicate with a Bluetooth device via RFCOMM protocol (SPP profile).
 * It calls the Java API of the Android OS, since there is no native API for bluetooth available.
 * <p>
 * This class does not provide a feature to pair devices. The user may do that manually in the
 * system settings of the Android device. Once a device is paired, you can communicate with it.
 * <p>
 * Please note that Qt 5.3 supports bluetooth on Android out of the box.
 * This class may be used as an alternative, specially on Qt 5.0 ... 5.2.
 * It will possibly also work in future versions.
 * <p>
 * Your project file must contain
 * QT += core androidextras
 * <p>
 * Your android.xml file must contain
 * <uses-permission android:name="android.permission.BLUETOOTH"/>
 */

class AndroidRfComm  {
    Q_DISABLE_COPY(AndroidRfComm)

public:

    /** Whether to encode text as latin1 instead of UTF-8. Default if UTF-8. */
    bool useLatin1;

    /** Terminator for lines of text, for sending. Default is a Unix line break. */
    QString sendTerminator;

    /** Terminator for lines of text, for receiving. Default is a Unix line break. */
    QString receiveTerminator;

    /**
     * Constructor.
     */
    AndroidRfComm();

    /**
     * Destructor, disconnects if not already disconnected.
     */
    virtual ~AndroidRfComm();

    /**
     * Check whether the Bluetooth interface of the Android device is enabled.
     * @return true if Bluetooth is enabled.
     */
    bool isEnabled();

    /**
     * Get a list of all paired remote devices with address and name.
     * The user can add additional devices in the "settings" of his Android device.
     * @return A map with the devices address as key and name as value.
     */
    QMap<QString,QString> getPairedDevices();

    /**
     * Open a connection to a remote device.
     * You can find available devices by iterating over the result of getPairedDevices().
     * @param nameOrAddress Name or address of the device, e.g. "HC-06" or "20:13:11:15:16:08". Using the address is better because it's unique.
     * @see isConnected() to check for success.
     */
    void connect(const QString& nameOrAddress);

    /**
     * Check if the android device is connected to the remote device.
     * @return true if the connection is open.
     */
    bool isConnected();

    /**
     * Send raw data from the Android device to the remote device.
     * @param data An array of bytes
     */
    void send(const QByteArray& data);

    /**
     * Send a line of text to the remote device.
     * @param text line of text to send. The terminator (line-break) gets appended automatically.
     * @see setSendTerminator to change the terminator. The default is "\n".
     * @see setLatin1Encoding to switch to latin1 encoding. The default is UTF-8.
     */
    void sendLine(const QString& text);

    /**
     * Query how many bytes have been received and can be read out now.
     * @return Number of bytes that are available for reading.
     */
    int available();

    /**
     * Receive a number of bytes from the remote device.
     * In case of a timeout, this method returns the bytes that have been received so far.
     * @param maxNumOfBytes Number of bytes that are expected.
     * @param waitMilliSeconds Maximum allowed time to wait for the number of bytes.
     * @return The received bytes.
     */
    QByteArray receive(const int maxNumOfBytes, const int waitMilliSeconds);

    /**
     * Receive a line of text from the remote device.
     * @param maxCharacters Maximum number of excpected characters.
     * @param waitMilliSeconds Maximum allowed time to wait for the terminator (line-break).
     * @return The received line of text, without terminator.
     * @see setReceiveTerminator to change the terminator. The default is "\n".
     * @see setLatin1Encoding to switch to latin1 encoding. The default is UTF-8.
     */
    QString receiveLine(const int maxCharacters, const int waitMilliSeconds);

    /**
     * Check whether the previous receive() or receiveLine() call ran into a timeout.
     * @return true if a timeout occured.
     */
    bool hasTimeout();

    /**
     * Close the open connection.
     * Does nothing, if the connection is already closed.
     */
    void disconnect();


private:

    /** JNI Environment */
    QAndroidJniEnvironment env;

    /** Reference to Java object android.bluetooth.BluetoothAdapter */
    QAndroidJniObject adapter;

    /** Reference to Java object android.bluetooth.BluetoothDevice */
    QAndroidJniObject device;

    /** Reference to Java object android.bluetooth.BluetoothSocket */
    QAndroidJniObject socket;

    /** Reference to Java object java.io.InputStream */
    QAndroidJniObject istream;

    /** Reference to Java object java.io.OutputStream */
    QAndroidJniObject ostream;

    /** Buffer for received bytes */
    QByteArray buffer;

    /** Indicator for receive timeout */
    bool timeout;

    /** Check for and log Java exception */
    void check(const char* method);

    /** Check for and log Java exception, also check if a Java object is valid */
    void check(const char* method, const QAndroidJniObject& obj);

    /** Read up to 1kB of received raw data into the buffer */
    int readIntoBuffer();
};

#endif // ANDROIDRFCOMM_H
