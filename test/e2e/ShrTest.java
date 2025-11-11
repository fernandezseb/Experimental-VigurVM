
public class ShrTest {
    public static void main(String[] args) {
        int[][] ints = {
                {1, 2, 0},
                {0, 3, 0},
                {256, 2, 64},
                {256, -2, 0},
                {255, 1, 127},
                {-256, 1, -128},
                {1024, 2, 256},
                {2048, 12, 0},
                {789512111, 2, 197378027},
                {2555555, 4, 159722},
                {2555555, 0, 2555555},
        };
        for (int [] row : ints) {
            int result = (row[0] >> row[1]);
            if (result != row[2]) {
                throw new TestValidationException("Expected result is: " + row[2] + "but was: " + result);
            }
        }
    }
}