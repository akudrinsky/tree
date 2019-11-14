void loadFromFile (char *filename)
{
    File file{};
    file = loadFile (filename);
    char *buffer = file.raw_data;
    char *arg = nullptr;

    if (*buffer == '{')
    {
        buffer = parseArg (buffer, &arg);
        createRoot (arg);

        if (*buffer == '{')
        {
            loadLeftSubTree (&buffer, arg, root);
            loadRightSubTree (&buffer, arg, root);
        }
    }
    else
    {
        perror ("File format error!");
    }
}

void loadLeftSubTree (char **buffer, char *arg, Node<T> *parent)
{
    *buffer = parseArg (*buffer, &arg);
    if (strcmp (arg, "@") == 0)
    {
        parent->left = nullptr;
        return;
    }
    else
    {
        auto node = newNode (arg);
        connectNodeLeft (parent, node);

        if (**buffer == '{')
        {
            loadLeftSubTree (buffer, arg, node);
            loadRightSubTree (buffer, arg, node);
        }
        else
        {
            node->left = nullptr;
            node->right = nullptr;
        }
    }
}

void loadRightSubTree (char **buffer, char *arg, Node<T> *parent)
{
    *buffer = parseArg (*buffer, &arg);
    if (strcmp (arg, "@") == 0)
    {
        parent->right = nullptr;
        return;
    }
    else
    {
        auto node = newNode (arg);
        connectNodeRight (parent, node);

        if (**buffer == '{')
        {
            loadLeftSubTree (buffer, arg, node);
            loadRightSubTree (buffer, arg, node);
        }
        else
        {
            node->left = nullptr;
            node->right = nullptr;
        }
    }
}