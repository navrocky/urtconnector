#ifndef QACCUMULATINGCONNECTION_H
#define QACCUMULATINGCONNECTION_H

#include <QObject>

/*! Accumulating connection.

Example:
\code
new QAccumulatingConnection(object, SIGNAL(stateChanged()), this,
                        SLOT(updateObjectInfo()), 500, QAccumulatingConnection::Periodically, this);
\endcode
*/

class QAccumulatingConnection : public QObject
{
    Q_OBJECT
public:
    /*! Emit mode */
    enum EmitMode
    {
        /*! Emits a signal after the lapse /p emitInterval ms since last call
            to emit slot. */
        Finally,

        /*! Periodically emits a signal every /p emitInterval ms until slot
            is called. */
        Periodically
    };

    /*! Constructs an accumulating connection. */
    QAccumulatingConnection(int emitInterval = 100, EmitMode mode = Finally, QObject* parent = NULL);

    /*! Constructs an accumulating connection and connects sender signal to receiver slot.  */
    QAccumulatingConnection(const QObject* sender, const char* signal,
        const QObject* receiver, const char* slot, int emitInterval = 100,
        EmitMode mode = Finally, QObject* parent = NULL);

    /*! Time interval in ms between call emitSignal() and emitting signal(). */
    int emitInterval() const {return emitInterval_;}

    /*! Current emit mode. */
    EmitMode emitMode() const {return emitMode_;}

signals:
    /*! Emitted signal after calling emitSignal() method. */
    void signal();

public slots:

    /*! Call this to emit signal() later. */
    void emitSignal();

    /*! Call this to emit signal() now, and reset emitting later. */
    void emitNow();

    /*! Resets emitting of pended signal(). */
    void resetEmit();

protected:
    virtual void timerEvent(QTimerEvent* e);
    
private:
    int emitInterval_;
    int timerId_;
    EmitMode emitMode_;
};

#endif