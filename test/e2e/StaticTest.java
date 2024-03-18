public class StaticTest {

    static class BaseClass {
        static int life = 42;
    }

    static class MainExtension extends BaseClass {
        static int six = 6;
    }

    public static void main(String[] args) {
        if (MainExtension.six != 6) {
            throw new TestValidationException("Static field read failure");
        }
        if (MainExtension.life != 42) {
            throw new TestValidationException("Static field read failure");
        }
    }
}