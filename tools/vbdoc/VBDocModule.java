

public class VBDocModule extends Object
{
    public Vector logicBlocks = null;
    public String author = null;
    public String todo = null;

    public VBDocModule()
    {
        logicBlocks = new Vector();
    } // Constructor

    public void addLogicBlock(VBDocLogicBlock block)
    {
        logicBlocks.add(block);
    } // addLogicBlock

    public void addAuthor(String name) throws DuplicateDefinitionError
    {
        if (author != null)
            throw(new DuplicateDefinitionError());
        author = name;
    } // addAuthor

    public void addTodo(String desc) throws DuplicateDefinitionError
    {
        if (todo != null)
            throw(new DuplicateDefinitionError());
        todo = name;
    } // addAuthor
} // VBDocModule
