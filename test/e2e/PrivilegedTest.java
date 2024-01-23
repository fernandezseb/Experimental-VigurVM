import java.security.PrivilegedAction;

import static java.security.AccessController.doPrivileged;

public class PrivilegedTest {
    public static void main(String[] args) {
        doPrivileged(new PrivilegedAction<Void>() {
            @Override
            public Void run() {
                return null;
            }
        });
    }
}