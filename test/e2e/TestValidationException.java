public class TestValidationException extends RuntimeException {

    public TestValidationException() {
    }

    public TestValidationException(String message) {
        super(message);
    }

    public TestValidationException(String message, Throwable cause) {
        super(message, cause);
    }

    public TestValidationException(Throwable cause) {
        super(cause);
    }

    public TestValidationException(String message, Throwable cause, boolean enableSuppression, boolean writableStackTrace) {
        super(message, cause, enableSuppression, writableStackTrace);
    }
}
