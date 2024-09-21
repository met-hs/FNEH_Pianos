import lombok.RequiredArgsConstructor;
import midi.MidiDeviceFinder;

import javax.sound.midi.*;
import java.io.Closeable;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

@RequiredArgsConstructor
public class MidiChannelListener implements Closeable {

    private static final int INITIAL_DELAY = 1000;
    private static final int DELAY = 5000;
    private static final TimeUnit DELAY_TIME_UNIT = TimeUnit.SECONDS;
    private MidiDevice midiDevice;
    private State state = State.SEARCHING;

    private final String deviceName;
    private final Receiver receiver;

    private final ScheduledExecutorService stateMachineExecutor = Executors.newScheduledThreadPool(1);


    public void start() {
        stateMachineExecutor.scheduleWithFixedDelay(this::runStep, INITIAL_DELAY, DELAY, DELAY_TIME_UNIT);
    }

    private void runStep() {
        switch (state) {
            case SEARCHING -> searchForDevice();
            case LISTENING -> {}
            case CLOSED -> System.out.println("Closed Midi Channel Listener for " + deviceName);
            case FAILED -> {
                System.out.println("Failed to open Midi device. This is fatal failure. " + deviceName);
                close();
            }
        }
    }

    @Override
    public void close() {
        if (midiDevice != null) {
            midiDevice.close();
        }
        state = State.CLOSED;
        stateMachineExecutor.shutdownNow();
    }

    private void searchForDevice() {

        try {
            midiDevice = MidiDeviceFinder.findTransmittingByName(deviceName);

            if (midiDevice == null) {
                System.out.println("Was not able to find Midi Device, will search again");
                return;
            }

            midiDevice.open();

            Transmitter transmitter = midiDevice.getTransmitter();

            transmitter.setReceiver(new Receiver() {

                @Override
                public void send(MidiMessage message, long timeStamp) {
                    receiver.send(message, timeStamp);
                    System.out.println(messageToString(message));
                }

                @Override
                public void close() {
                    state = State.LISTENING;
                }


                private String messageToString(MidiMessage message) {
                    StringBuilder sb = new StringBuilder();
                    sb.append("Status: ").append(message.getStatus()).append(", ");
                    if (message instanceof ShortMessage) {
                        ShortMessage sm = (ShortMessage) message;
                        sb.append("Command: ").append(sm.getCommand()).append(", ");
                        sb.append("Data1: ").append(sm.getData1()).append(", ");
                        sb.append("Data2: ").append(sm.getData2());
                    }
                    return sb.toString();
                }

            });

            state = State.LISTENING;
        } catch (MidiUnavailableException e) {
            state = State.FAILED;
        }
    }

    public enum State {
        SEARCHING,
        LISTENING,
        CLOSED,
        FAILED
    }
}
