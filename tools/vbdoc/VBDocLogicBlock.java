/**
 * This class abstractly gathers data about a logic block (function, sub, or
 *  command), and stores it so VBDocOutputable objects can retrieve it later.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

import java.util.Vector;

public abstract class VBDocLogicBlock extends Object
{
    protected String syntax = null;
    protected Vector compatibility = null;
    protected Vector bugs = null;
    protected String notes = null;
    protected String returns = null;

    public VBDocLogicBlock(String type, String name)
    {
        bugs = new Vector();
        compatibility = new Vector();
    } // Constructor

    public void finalize() throws Throwable
    {
        compatibility.setSize(0);
        compatibility.trimToSize();
        compatibility = null;
        bugs.setSize(0);
        bugs.trimToSize();
        bugs = null;
    } // finalize

    public void addSyntax(String desc) throws DuplicateDefinitionError
    {
        if (syntax != null)
            throw(new DuplicateDefinitionError());
        syntax = desc;
    } // addSyntax

    public void addCompatibility(String desc)
    {
        compatibility.add(desc);
    } // addCompatibility

    public void addBugs(String desc)
    {
        bugs.add(desc);
    } // addBugs

    public void addSeeAlso(String name, String libraryName)
    {
    } // addSeeAlso

    public void addParam(String paramName, String desc)
    {
    } // addParam

    public void addReturns(String desc)
    {
        if (returns != null)
            throw(new DuplicateDefinitionError());
        returns = desc;
    } // addReturns

    public void addThrows(String name, String desc)
    {
    } // addThrows

    public void addNotes(String name, String desc)
    {
        if (notes != null)
            throw(new DuplicateDefinitionError());
        notes = desc;
    } // addNotes

    public abstract String getBlockType();
} // VBDocLogicBlock

// end of VBDocLogicBlock.java ...
