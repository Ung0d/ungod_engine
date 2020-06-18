namespace uedit
{
    template<typename ... T>
    void ActionManager::action(const std::function<void(T...)>& execute, const std::function<void(T...)>& undo, T... t)
    {
        mEFrame->notifyProjectChanged();
        mActionManager.execute(execute, undo, std::forward<T>(t)...);
    }
}
