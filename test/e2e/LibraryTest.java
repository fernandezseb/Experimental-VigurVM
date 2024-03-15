import java.io.Serializable;

public class LibraryTest {
    public static void main(String[] args) {

        /*
         * Class tests
         */
        if (!Number.class.isAssignableFrom(Integer.class)) {
            throw new RuntimeException();
        }

        if (Integer.class.isAssignableFrom(Number.class)) {
            throw new RuntimeException();
        }

        if (Integer.class.isInterface()) {
            throw new RuntimeException();
        }

        if (!Serializable.class.isInterface()) {
            throw new RuntimeException();
        }


    }
}