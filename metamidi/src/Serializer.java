import java.util.*;

public class Serializer {

    private static final int PRE_PIANO_BIT = 0;
    private static final int PRE_NOTE_BIT = 1;

    public void serialize(final Map<FNEH_Piano, Map<Integer, Integer>> activeNotes) {
        List<Byte> bytes = new ArrayList<>();
        activeNotes.forEach(this::serialize);
    }

    private BitSet serialize(FNEH_Piano fnehPiano, Map<Integer, Integer> activeNotes) {
        int numberOfBits = 1 // bit required to encode piano or key
                + activeNotes.size()
                * (1         //bit required to encode it's a key
                + numberOfBitsPerNotOf(fnehPiano));

        BitSet bitSet = new BitSet(numberOfBits);

        bitSet.set(0, PRE_PIANO_BIT);
        bitSet.set(1, encodePiano(fnehPiano));

        int bitsSoFar = 4;
        bitSet.set(bitsSoFar, );
        int code = PRE_PIANO_BIT;

        code = PRE_NOTE_BIT << 3; //make room to add the piano code
        code = code |



        return bitSet;
    }

    private int encodePiano(FNEH_Piano piano) {
        return switch (piano) {
            case FNEH_Piano.FNEH_88_KEY -> 1;
            case FNEH_63_KEY -> 2;
            case FNEH_49_KEY -> 3;
            case N_OVATION_25_KEY -> 4;
            case MPK_MINI_25_KEY -> 5;
        };
    }

    private int numberOfBitsPerNotOf(FNEH_Piano piano) {
        return switch (piano) {
            case FNEH_Piano.FNEH_88_KEY -> 7;
            case FNEH_63_KEY, FNEH_49_KEY -> 6;
            case N_OVATION_25_KEY, MPK_MINI_25_KEY -> 5;
        };
    }

    private static BitSet setBitsAt(int number, int size) {
        if (number >= 1 << size) {
            throw new IndexOutOfBoundsException("Can only cover up to 7 pianos. " +
                    "This is just a limit in the encoding and can be increased. " +
                    "Please reach out to me to help.");
        }
        BitSet bitSet = new BitSet(size);

        for (int i = 0; i < size; i++) {
            if ((number & (1 << i)) != 0) {
                bitSet.set(i);
            }
        }

        return bitSet;
    }


}
