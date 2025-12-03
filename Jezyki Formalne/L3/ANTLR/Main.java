import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;
import java.io.*;
import java.util.*;

public class Main {

    private static final long GF = 1234577L;

    private static long mod(long x) {
        if (x < 0) return (GF + (x % GF)) % GF;
        else return x % GF;
    }

    private static long mod2(long x) {
        long m = GF - 1;
        x %= m;
        if (x < 0) x += m;
        return x;
    }

    private static long power(long b, long e) {
        e = mod2(e);
        long base = mod(b);
        long res = 1L;
        while (e > 0) {
            if ((e & 1L) == 1L) res = (res * base) % GF;
            base = (base * base) % GF;
            e >>= 1;
        }
        return res;
    }

    private static long divMod(long a, long b) {
        b = mod(b);
        if (b == 0) {
            System.out.println("Dzielenie przez 0!!!");
            return 0L;
        }
        a = mod(a);
        long inv = power(b, GF - 2);
        return (a * inv) % GF;
    }

    public static class LogicVisitor extends CalculatorBaseVisitor<Long> {

        @Override
        public Long visitPrintExpr(CalculatorParser.PrintExprContext ctx) {
            long value = visit(ctx.expr());
            System.out.println("\nWynik: " + mod(value));
            return value;
        }

        @Override
        public Long visitBlank(CalculatorParser.BlankContext ctx) {
            return 0L;
        }

        @Override
        public Long visitAddExpr(CalculatorParser.AddExprContext ctx) {
            int childCount = ctx.getChildCount();
            long value = visit(ctx.getChild(0));
            for (int i = 1; i < childCount; i += 2) {
                ParseTree opNode = ctx.getChild(i);
                ParseTree rightNode = ctx.getChild(i + 1);
                int type = ((TerminalNode) opNode).getSymbol().getType();
                long right = visit(rightNode);
                if (type == CalculatorParser.PLUS) {
                    System.out.print("+ ");
                    value = mod(value + right);
                } else { // MINUS
                    System.out.print("- ");
                    value = mod(value - right);
                }
            }
            return value;
        }

        @Override
        public Long visitMulExpr(CalculatorParser.MulExprContext ctx) {
            int childCount = ctx.getChildCount();
            long value = visit(ctx.getChild(0));
            for (int i = 1; i < childCount; i += 2) {
                ParseTree opNode = ctx.getChild(i);
                ParseTree rightNode = ctx.getChild(i + 1);
                int type = ((TerminalNode) opNode).getSymbol().getType();
                long right = visit(rightNode);
                if (type == CalculatorParser.STAR) {
                    System.out.print("* ");
                    value = mod(value * right);
                } else { // SLASH
                    System.out.print("/ ");
                    value = divMod(value, right);
                }
            }
            return value;
        }

        @Override
        public Long visitPowExpr(CalculatorParser.PowExprContext ctx) {
            long left = visit(ctx.left);
            if (ctx.right != null) {
                long right = visit(ctx.right);
                long res = power(left, right);
                System.out.print("^ ");
                return res;
            }
            return left;
        }

        @Override
        public Long visitUnaryMinus(CalculatorParser.UnaryMinusContext ctx) {
            long v = visit(ctx.unaryExpr());
            return -v;
        }

        @Override
        public Long visitPrimaryExpr(CalculatorParser.PrimaryExprContext ctx) {
            return visit(ctx.primary());
        }

        @Override
        public Long visitNumber(CalculatorParser.NumberContext ctx) {
            long v = Long.parseLong(ctx.NUMBER().getText());
            long r = mod(v);
            System.out.print(r + " ");
            return r;
        }

        @Override
        public Long visitParens(CalculatorParser.ParensContext ctx) {
            return visit(ctx.expr());
        }
    }

    public static void main(String[] args) throws Exception {
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        String line;
        LogicVisitor visitor = new LogicVisitor();

        while ((line = reader.readLine()) != null) {
            CharStream cs = CharStreams.fromString(line + "\n");
            CalculatorLexer lexer = new CalculatorLexer(cs);
            CommonTokenStream tokens = new CommonTokenStream(lexer);
            CalculatorParser parser = new CalculatorParser(tokens);

            try {
                CalculatorParser.LineContext tree = parser.line();
                if (parser.getNumberOfSyntaxErrors() == 0) {
                    visitor.visit(tree);
                } else {
                    System.out.println("Błąd");
                }
            } catch (Exception ex) {
                System.err.println("Błąd parsowania: " + ex.getMessage());
                System.out.println();
            }
        }
    }
}