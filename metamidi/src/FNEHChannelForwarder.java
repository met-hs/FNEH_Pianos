import midi.MidiDeviceFinder;

import javax.sound.midi.*;
import java.io.IOException;

public class FNEHChannelForwarder {

    private static final String DEVICE = "UM-ONE";

    public static void main(String[] args) throws MidiUnavailableException, IOException, InvalidMidiDataException, InterruptedException {

        MidiDevice arduino = MidiDeviceFinder.findReceivingDevice("Arduino Due");
        assert arduino != null;
        arduino.open();

        // Play a note (Middle C, MIDI note number 60)
        int note = 60; // MIDI note number for Middle C
        int velocity = 100; // Velocity (volume)
        while(true) {
            ShortMessage noteOn = new ShortMessage();
            noteOn.setMessage(ShortMessage.NOTE_ON, 1, note, velocity); // Channel 1
            arduino.getReceiver().send(noteOn, -1);
            System.out.println("Sending Node On");
            Thread.sleep(1000);
            ShortMessage noteOff = new ShortMessage();
            noteOff.setMessage(ShortMessage.NOTE_OFF, 1, note, 0); // Channel 1
            arduino.getReceiver().send(noteOff, -1);
//        MidiChannelListener midiChannelListener = new MidiChannelListener("Arduino Due", arduino.getReceiver());
//        midiChannelListener.start();
            System.out.println("Sending Node Off");
            Thread.sleep(1000);
        }

        // Keep the application running to receive messages
      //  System.out.println("Listening for MIDI messages. Press Enter to exit.");
      //  System.in.read();

        // Close the MIDI device
     //   arduino.close();
       // midiChannelListener.close();
    }


}
