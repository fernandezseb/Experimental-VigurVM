
public class DivMulTest {
    public static void main(String[] args) {
        int[][] ints = {
                {1, 2, 0, 2},
                {0, 3, 0, 0},
                {256, 2, 128, 512},
                {256, -2, -128, -512},
                {255, 1, 255, 255},
                {-256, 1, -256, -256},
                {1024, 2, 512, 2048},
                {2048, 12, 170, 24576},
                {789512111, 2, 394756055, 1579024222},
                {2555555, 4, 638888, 10222220},
                {0, 2555555, 0, 0},
        };
        for (int [] row : ints) {
            int resultDiv = (row[0] / row[1]);
            int resultMul = (row[0] * row[1]);
            if (resultDiv != row[2]) {
                throw new TestValidationException("Expected result for division is: " + row[2] + "but was: " + resultDiv);
            }

            if (resultMul != row[3]) {
                throw new TestValidationException("Expected result for multiplication is: " + row[3] + "but was: " + resultMul);
            }
        }
    }
}