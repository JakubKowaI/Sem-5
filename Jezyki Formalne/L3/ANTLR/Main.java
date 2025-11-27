import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.ParseTree;
import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Main {

    private static final int GF = 1234577;

    private static int mod(int x) {
        if (x < 0) return (GF + (x % GF)) % GF;
        else return x % GF;
    }

    // Klasa Visitora z logiką
    public static class LogicVisitor extends CalculatorBaseVisitor<Integer> {

        @Override
        public Integer visitPrintExpr(CalculatorParser.PrintExprContext ctx) {
            Integer value = visit(ctx.expr());
            System.out.println("\nWynik: " + value);
            return value;
        }

        @Override
        public Integer visitNumber(CalculatorParser.NumberContext ctx) {
            int value = Integer.parseInt(ctx.getText());
            System.out.print(value + " ");
            return value;
        }

        @Override
        public Integer visitNegativeNumber(CalculatorParser.NegativeNumberContext ctx) {
            int value = Integer.parseInt(ctx.NUMBER().getText());
            int result = mod(-value);
            System.out.print(result + " ");
            return result;
        }

        @Override
        public Integer visitAddSub(CalculatorParser.AddSubContext ctx) {
            int left = visit(ctx.left);
            int right = visit(ctx.right);
            int result;
            if (ctx.op.getType() == CalculatorParser.PLUS) {
                result = mod(left + right);
                System.out.print("+ ");
            } else {
                result = mod(left - right);
                System.out.print("- ");
            }
            return result;
        }

        @Override
        public Integer visitMulDiv(CalculatorParser.MulDivContext ctx) {
            int left = visit(ctx.left);
            int right = visit(ctx.right);
            int result;
            if (ctx.op.getType() == CalculatorParser.STAR) {
                result = mod(left * right);
                System.out.print("* ");
            } else {
                result = mod(left / right);
                System.out.print("/ ");
            }
            return result;
        }

        @Override
        public Integer visitPower(CalculatorParser.PowerContext ctx) {
            int left = visit(ctx.left);
            int right = visit(ctx.right);
            int result = mod((int) Math.pow(left, right));
            System.out.print("^ ");
            return result;
        }

        @Override
        public Integer visitParens(CalculatorParser.ParensContext ctx) {
            return visit(ctx.expr());
        }
    }

    public static void main(String[] args) throws Exception {
        // Czytanie wejścia linia po linii (tryb interaktywny)
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        String inputLine;

        while ((inputLine = reader.readLine()) != null) {
            // Dodajemy \n, ponieważ readLine() go ucina, a gramatyka wymaga NEWLINE na końcu
            CharStream input = CharStreams.fromString(inputLine + "\n");

            CalculatorLexer lexer = new CalculatorLexer(input);
            CommonTokenStream tokens = new CommonTokenStream(lexer);
            CalculatorParser parser = new CalculatorParser(tokens);

            // Ważne: Używamy reguły 'line' zamiast 'input'.
            // 'input' oczekuje EOF, a 'line' parsuje pojedyncze wyrażenie.
            ParseTree tree = parser.line();

            LogicVisitor visitor = new LogicVisitor();
            visitor.visit(tree);
        }
    }
}