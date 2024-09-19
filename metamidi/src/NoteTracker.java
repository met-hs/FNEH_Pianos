import javax.sound.midi.MidiMessage;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class NoteTracker {
    Map<Integer, Integer> activeNotes = new ConcurrentHashMap<>();

    public void track(MidiMessage m) {

        // the message type (eg note on/off)
        int status = m.getStatus();
        // data like which note (c# vs c etc)
        int msgData = m.getMessage()[1];

        if (status == 144) noteOn(msgData);
        if (status == 128) noteOff(msgData);

        System.out.println(activeNotes);

    }

    void noteOn(int noteNumber) {
        activeNotes.put(noteNumber, noteNumber);
    }

    void noteOff(int noteNumber) {
        activeNotes.remove(noteNumber);
    }
}