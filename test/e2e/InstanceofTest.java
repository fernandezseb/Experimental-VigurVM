
public class InstanceofTest {
    public static void main(String[] args) {
        String brol = "";
        Object o = new Object();
        Object oNull = null;
//         char[] charArr = new char[2];
        if (!(brol instanceof String))
            throw new RuntimeException();
        if (!(brol instanceof Object))
             throw new RuntimeException();
        if (o instanceof String)
             throw new RuntimeException();
        if (oNull instanceof String)
             throw new RuntimeException();
//         if (!(brol instanceof Serializable))
//             throw new RuntimeException();
//         if (!(charArr instanceof char[]))
//             throw new RuntimeException();
    }
}