/**
 * This class abstractly gathers data about a logic block (function, sub, or
 *  command), and stores it so VBDocOutputable objects can retrieve it later.
 *
 *   Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

import java.util.Vector;
import java.util.Hashtable;

public class VBDocLogicBlock extends Object
{
    public String blockType = null;
    public String syntax = null;
    public Vector compatibility = null;
    public Vector bugs = null;
    public String notes = null;
    public String returns = null;
    public Hashtable seeAlso = null;
    public Hashtable params = null;
    public Hashtable runtimeErrs = null;


    public VBDocLogicBlock(String blockType)
    {
        bugs = new Vector();
        compatibility = new Vector();
        seeAlso = new Hashtable();
        params = new Hashtable();
        this.blockType = blockType;
    } // Constructor

    public void finalize() throws Throwable
    {
        compatibility.setSize(0);
        compatibility.trimToSize();
        compatibility = null;
        bugs.setSize(0);
        bugs.trimToSize();
        bugs = null;
        seeAlso.clear();
        seeAlso = null;
        params.clear();
        params = null;
        runtimeErrs.clear();
        runtimeErrs = null;
    } // finalize


    public void addSyntax(String desc) throws DuplicateDefinitionError
    {
        if (syntax != null)
            throw(new DuplicateDefinitionError());
        syntax = desc;
    } // addSyntax


    public void addCompatibility(String desc)
    {
        compatibility.addElement(desc);
    } // addCompatibility


    public void addBugs(String desc)
    {
        bugs.addElement(desc);
    } // addBugs


    public void addSeeAlso(String name, String libraryName)
    {
        seeAlso.put(libraryName, name);
    } // addSeeAlso


    public void addParam(String paramName, String desc)
    {
        params.put(paramName, desc);
    } // addParam


    public void addReturns(String desc) throws DuplicateDefinitionError
    {
        if (returns != null)
            throw(new DuplicateDefinitionError());
        returns = desc;
    } // addReturns


    public void addThrows(String name, String desc)
    {
        runtimeErrs.put(name, desc);
    } // addThrows


    public void addNotes(String desc) throws DuplicateDefinitionError
    {
        if (notes != null)
            throw(new DuplicateDefinitionError());
        notes = desc;
    } // addNotes

} // VBDocLogicBlock

// end of VBDocLogicBlock.java ...
