#include "minishell.h"


char *get_cmd_path(char *cmd, t_env *env_list)
{
    char **paths;
    char *path_env;
    char *full_path;
    int i;
    struct stat stat_buf;

    if (!cmd)
        return NULL;

    /* Handle absolute or relative paths: return as-is, let execve report errno */
    if (ft_strchr(cmd, '/'))
    {
        return ft_strdup(cmd);
    }

    /* Search in PATH */
    path_env = get_env_value("PATH", env_list);
    if (!path_env)
        return NULL;

    paths = ft_split(path_env, ':');
    if (!paths)
        return NULL;

    i = 0;
    while (paths[i])
    {
        full_path = str_concat_three(paths[i], "/", cmd);
        if (full_path && stat(full_path, &stat_buf) == 0)
        {
            /* If it's a directory, still return it; caller will handle and report 126 */
            free_strs(paths);
            return full_path;
        }
        free(full_path);
        i++;
    }
    free_strs(paths);
    return NULL;
}

void free_strs(char **strs)
{
    int i = 0;
    if (!strs) return;
    while (strs[i])
        free(strs[i++]);
    free(strs);
}

int execute_command(char **args, t_env **env)
{
    pid_t pid;
    int status;
    char **envp;

    pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return -1;
    }
    if (pid == 0)
    {
        envp = envlist_to_array(*env);
        if (!envp)
            exit(EXIT_FAILURE);
        execve(args[0], args, envp);
        perror("execve failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
    }
    return 0;
}
