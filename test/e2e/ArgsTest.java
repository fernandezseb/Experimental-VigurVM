
public class ArgsTest {
    public static void main(String[] args) {

        if (args.length != 0) {
            throw new RuntimeException("Wrong amount of args");
        }

        if (!args[0].equals("arg1")) {
            throw new RuntimeException("Args is not correct string");
        }


    }
}