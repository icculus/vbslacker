import java.io.*;

public class HexDump
{

    public static void dumpAsciiChars(int[] b, int blen)
    {
        int lineBytes;

        System.out.print("            ");
        for (lineBytes = 0; lineBytes < blen; lineBytes++)
        {
            if ((b[lineBytes] >= 32) && (b[lineBytes] <= 126))
                System.out.print((char) b[lineBytes]);
            else
                System.out.print(".");
        } // for

        System.out.println();
    } // dumpAsciiChars


    public static void dumpHex(BufferedInputStream buf)
    {
        int lineBytes = 0;
        int i;
        int[] b = new int[16];
        String s;

        try
        {
            while (buf.available() > 0)
            {
                b[lineBytes] = buf.read();

                s = Integer.toHexString(b[lineBytes]);
                if (s.length() < 2)
                    s = "0" + s;

                System.out.print(s.toUpperCase() + " ");

                lineBytes++;
                if (lineBytes == b.length)
                {
                    dumpAsciiChars(b, b.length);
                    lineBytes = 0;
                } // if
            } // while

            if (lineBytes > 1)   // unfilled line?
            {
                for (i = lineBytes; i < b.length; i++)  // pad out.
                    System.out.print("   ");
                dumpAsciiChars(b, lineBytes);      // dump chars.
            } // if

            System.out.println();

        } // try
        catch (IOException e)
        {
            System.out.println("IOException : " + e.getMessage());
            return;
        } // catch
    } // dumpHex

    public static void main(String[] args)
    {
        BufferedInputStream buf;

        for (int i = 0; i < args.length; i++)
        {
           try
           {
               buf = new BufferedInputStream(new FileInputStream(args[i]));
           } // try
           catch (IOException e)
           {
               System.out.println("IOException on " + args[i] + " : " + 
                                    e.getMessage());
               buf = null;
           } // catch

           if (buf != null)
           {
               System.out.println("File '" + args[i] + "' ...");

               dumpHex(buf);

               try
               {
                   buf.close();
               } // try
               catch (IOException e)
               {
                   // don't care...
               } // catch

           } // if
        } // for
    } // main
} // HexDump

// end of HexDump.java ...


