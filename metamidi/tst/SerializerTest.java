import org.junit.jupiter.api.Test;

import java.util.*;

import static org.junit.jupiter.api.Assertions.*;

class SerializerTest {

    @Test
    void seriaize() {
        Map<FNEH_Piano, Set<Integer>> activeNotes = new HashMap<>();
        activeNotes.put(FNEH_Piano.N_OVATION_25_KEY, Set.of(16, 18, 20, 1, 3));
        activeNotes.put(FNEH_Piano.MPK_MINI_25_KEY, Set.of(2, 4, 6, 20, 24));
        activeNotes.put(FNEH_Piano.FNEH_88_KEY, Set.of(0, 87, 84, 83, 12, 1, 2, 3, 10));
        activeNotes.put(FNEH_Piano.FNEH_49_KEY, Set.of(0, 47, 48, 46, 12, 1, 2, 5, 9));
        activeNotes.put(FNEH_Piano.FNEH_63_KEY, Set.of(0, 22, 55, 46, 62, 1, 2, 5, 9));
        Serializer serializer = new Serializer();
        byte[] serialize = serializer.serialize(activeNotes);
        System.out.println(activeNotes);
        System.out.println(Arrays.toString(serialize));
    }

    @Test
    void seriaize_empty() {
        Map<FNEH_Piano, Set<Integer>> activeNotes = new HashMap<>();
        Serializer serializer = new Serializer();
        byte[] serialize = serializer.serialize(activeNotes);
        assertEquals(0, serialize.length);
    }

    @Test
    void seriaize_justOneNote() {
        Map<FNEH_Piano, Set<Integer>> activeNotes = new HashMap<>();
        activeNotes.put(FNEH_Piano.N_OVATION_25_KEY, Set.of(1));
        Serializer serializer = new Serializer();
        byte[] serialize = serializer.serialize(activeNotes);
        assertEquals(2, serialize.length);
        assertEquals(33, serialize[0]);
        assertEquals(1, serialize[1]);
        print(serialize);
    }

    @Test
    void seriaize_justOneNoteOf0() {
        Map<FNEH_Piano, Set<Integer>> activeNotes = new HashMap<>();
        activeNotes.put(FNEH_Piano.FNEH_88_KEY, Set.of(0));
        Serializer serializer = new Serializer();
        byte[] serialize = serializer.serialize(activeNotes);
        assertEquals(2, serialize.length);
        assertEquals(-128, serialize[0]);
        assertEquals(1, serialize[1]);
        print(serialize);
    }

    private static void print(byte[] serialize) {
        for (byte b : serialize) {
            System.out.println(String.format("%8s", Integer.toBinaryString(b & 0xFF)).replace(' ', '0'));
        }
    }


}