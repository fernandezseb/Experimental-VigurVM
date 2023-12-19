
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.util.function.Supplier;

public class Main {

    @Retention(RetentionPolicy.RUNTIME)
    @Target(ElementType.TYPE_USE)
    public @interface JsonElement {
        public String key() default "";
    }

    public static void main(String[] args) {
        Supplier<@JsonElement Integer> integerSupplier = () -> 42;
    }

}
