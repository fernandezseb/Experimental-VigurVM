import java.io.Serializable;

public class InstanceofTest {
    public static void main(String[] args) {
        String brol = "";
        Object o = new Object();
        Object oNull = null;
//         char[] charArr = new char[2];
        if (!(brol instanceof String))
            throw new TestValidationException();
        if (!(brol instanceof Object))
             throw new TestValidationException();
        if (o instanceof String)
             throw new TestValidationException();
        if (oNull instanceof String)
             throw new TestValidationException();
        if (!(brol instanceof Serializable))
            throw new TestValidationException();
//         if (!(charArr instanceof char[]))
//             throw new RuntimeException();
    }
}