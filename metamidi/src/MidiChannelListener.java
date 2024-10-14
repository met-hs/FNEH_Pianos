import midi.MidiDeviceFinder;

import javax.sound.midi.*;
import java.io.Closeable;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class MidiChannelListener implements Closeable {

    private static final int INITIAL_DELAY = 1;
    private static final int DELAY = 5;
    private static final TimeUnit DELAY_TIME_UNIT = TimeUnit.SECONDS;
    private MidiDevice midiDevice;
    private State state = State.SEARCHING;
    private int outputChannel;

    private final String deviceName;
    private final Receiver receiver;

    private final ScheduledExecutorService stateMachineExecutor = Executors.newScheduledThreadPool(1);

    public MidiChannelListener(String deviceName, Receiver receiver, int outputChannel) {
        this.deviceName = deviceName;
        this.receiver = receiver;
        this.outputChannel = outputChannel;
    }

    public void start() {
        stateMachineExecutor.scheduleWithFixedDelay(this::runStep, INITIAL_DELAY, DELAY, DELAY_TIME_UNIT);
    }

    private void runStep() {
        System.out.println("running the step ");
        switch (state) {
            case SEARCHING -> searchForDevice();
            case LISTENING -> {
            }
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
                    try {
                        receiver.send(changeChannel(message, outputChannel), timeStamp);
                    } catch (InvalidMidiDataException e) {
                        throw new RuntimeException(e);
                    }
                    //System.out.println(messageToString(message));
                }

                @Override
                public void close() {
                    state = State.LISTENING;
                }

                private MidiMessage changeChannel(MidiMessage message, int channel) throws InvalidMidiDataException {
                    // Change the channel
                    if (message instanceof ShortMessage) {
                        ShortMessage sm = (ShortMessage) message;
                        int command = sm.getCommand();

                        // Modify the channel of the message
                        ShortMessage newMessage = new ShortMessage();
                        newMessage.setMessage(command, channel, sm.getData1(), sm.getData2());
                        return newMessage;
                    } else {
                        // Send unmodified messages (if they are not ShortMessage)
                        return message;
                    }
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
