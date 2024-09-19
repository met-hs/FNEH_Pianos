import javax.sound.midi.*;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class MidiChannelListener {

    public static void main(String[] args) {
        try {
            // Obtain a MIDI device that is available for input
            MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();
            MidiDevice inputDevice = null;

            for (MidiDevice.Info info : infos) {
                MidiDevice device = MidiSystem.getMidiDevice(info);
                device.open();
                System.out.println("Device: " + info.getName());
                System.out.println("    Description: " + info.getDescription());
                System.out.println("    Vendor: " + info.getVendor());
                System.out.println("    Version: " + info.getVersion());
                System.out.println("    Max Transmitters: " + device.getMaxTransmitters());
                System.out.println("    Max Receivers: " + device.getMaxReceivers());
                System.out.println("    Receivers: " + device.getReceivers());
                System.out.println("    Transmitters: " + device.getTransmitters());
                System.out.println();
                if(info.getName().equals("Piano_LEDs")) {
                    if (device.getMaxTransmitters() != 0) {
                        inputDevice = device;
                        break;
                    }
                }
            }
            if (inputDevice == null) {
                System.out.println("No suitable MIDI input device found.");
                return;
            }
            Transmitter transmitter1 = inputDevice.getTransmitter();

            // Open the MIDI device
            inputDevice.open();

            // Create a receiver to handle incoming MIDI messages
            Receiver receiver = new Receiver() {
                final NoteTracker noteTracker = new NoteTracker();

                @Override
                public void send(MidiMessage message, long timeStamp) {
                    noteTracker.track(message);
                }

                @Override
                public void close() {
                    // Cleanup resources if needed
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
            };

            transmitter1.setReceiver(receiver);

            // Keep the application running to receive messages
            System.out.println("Listening for MIDI messages. Press Enter to exit.");
            System.in.read();

            // Close the MIDI device
            inputDevice.close();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }


}
