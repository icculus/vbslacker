/**
 * This class reads specified VBDoc format files and converts them to a
 *  given documentation format. This format depends on what implementation of
 *  VBDocOutputable is used.
 *
 *  This is quick and dirty hacked code. Poorly programmed, at best. --ryan.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

import java.io.*;
import java.util.*;

public class VBDoc extends Object
{
    private static Vector modules = null;
    private static VBDocModule theMod = null;
    private static VBDocLogicBlock theLogic = null;
    private static boolean inLib = false;
    private static boolean inModule = false;
    private static boolean inLogic = false;

    private static void setupToker(StreamTokenizer toker)
    {
        toker.resetSyntax();
        toker.eolIsSignificant(false);
        toker.slashSlashComments(false);
        toker.slashStarComments(false);
        toker.commentChar('#');
        toker.whitespaceChars(0x00, 0x20);
        toker.wordChars('0', '9');
        toker.wordChars('a', 'z');
        toker.wordChars('A', 'Z');
        toker.wordchars('@', '@');
    } // setupToker


    private static String toNextAtChar(StreamTokenizer toker)
    {
        StringBuffer sb = new StringBuffer();

        toker.nextToken();
        while ((toker.ttype == StreamTokenizer.TT_WORD) && 
               (toker.sval.charAt(0) != '@'))
        {
            sb.append(toker.sval);
            sb.append(" ");
            toker.nextToken();
        } // while

        toker.pushBack();
        sb.setLength(0);
    } // toNextAtChar


    private static void dealWithWord(StreamTokenizer toker) throws Exception
    {
        if (toker.sval.equalsIgnoreCase("@MODULE"))
        {
            if (inModule)
                throw(new Exception("@MODULE inside @MODULE"));
            else
            {
                inModule = true;
                theMod = new VBDocModule();
                modules.addElement(theMod);
                toker.nextToken();
                theMod.addName(toker.sval);
            } // else
        } // if

        else if (toker.sval.equalsIgnoreCase("@ENDMODULE"))
        {
            if (inLogic == true)
                throw(new Exception("@ENDMODULE inside @COMMAND"));

            if (inModule == false)
                throw(new Exception("@ENDMODULE without @MODULE"));

            inModule = false;
            theMod = null;
        } // else if

        else if (toker.sval.equalsIgnoreCase("@AUTHOR"))
        {
            if (inModule == false)
                throw(new Exception("@AUTHOR outside @MODULE"));
            toker.nextToken();
            theMod.addAuthor(toNextAtChar(toker));
        } // else if

        else if (toker.sval.equalsIgnoreCase("@COMMAND"))
        {
            if (inLogic == true)
                throw(new Exception("@COMMAND inside @COMMAND"));
            else
            {
                if (theMod == null)
                    throw(new Exception("@COMMAND outside @MODULE"));

                inLogic = true;
                toker.nextToken();
                theLogic = new VBDocLogicBlock(toker.sval);
                theMod.addLogicBlock(theLogic);
            } // else
        } // else if

        else if (toker.sval.equalsIgnoreCase("@ENDCOMMAND"))
        {
            if (inLogic == false)
                throw(new Exception("@ENDCOMMAND without @COMMAND"));

            inLogic = false;
            theLogic = null;
        } // else if

    } // dealWithWord


    private static void grabTokens(StreamTokenizer toker)
    {
        while (true)
        {
            toker.nextToken();
            switch (toker.ttype)
            {
                case StreamTokenizer.TT_EOF:
                    return;
                case StreamTokenizer.TT_WORD:
                    dealWithWord(toker);
            } // switch
        } // while
    } // grabToken


        /**
         * Opens specified file, and ...uh, parses it. Yeah.
         */
    private static void parseFile(String fileName) throws IOException
    {
        BufferedReader br = new LineInputReader(new FileReader(fileName));
        StreamTokenizer toker = new StreamTokenizer(br);

        setupToker(toker);
        grabTokens(toker);
    } // parseFile


        /**
         * The mainline.
         */
    public static void main(String args[])
    {
        int i;

        modules = new Vector();

        try
        {
            for (i = 0; i < args.length; i++)
                parseFile(args[i]);
        } // try
        catch (Exception e)
        {
            System.err.println("Exception: " + e.getMessage());
            System.err.println("  - Operation did NOT complete successfully.");
        } // catch
    } // main
} // VBDoc

// end of VBDoc.java ...
