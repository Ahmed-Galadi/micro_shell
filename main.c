
#include <libc.h>

int ft_put_err(char *err, char *arg)
{
	while (err && *err)
		write(2, err++, 1);
	while (arg && *arg)
		write(2, arg++, 1);
	write(2, "\n", 1);
	return (1);
}

int exit_fatal()
{
	write(2, "error: fatal\n", strlen("error: fatal\n"));
	exit(1);
}

int cd(char **s, int size)
{
	if (size != 2)
		return (ft_put_err("error: cd: bad arguments", NULL));
	else if (chdir (s[1]) == -1)
		return (ft_put_err("error: cd: cannot change directory to ", s[1]));
	return (0);
}

int exec_(char **av, int i, char **env)
{
	pid_t pid;
	int fd[2];
	int ret = 0;
	int check = 0;
	if (av[i] && (!strcmp(av[i], "|")))
		check = 1;
	if (!strcmp(av[0], "cd"))
		return (cd(av, i));
	if (check && pipe(fd) < 0)
		exit_fatal();
	pid = fork();
	if(pid < 0)
		exit_fatal();
	if (!pid)
	{
		av[i] = NULL;
		if (check && (dup2(fd[1], 1) < 0 || close(fd[0]) < 0|| close(fd[1]) < 0))
			exit_fatal();
		execve(*av, av, env);
		ft_put_err("error: cannot execute ", av[0]);
		exit (1);
	}
	waitpid(pid, &ret, 0);
	if (check && (dup2(fd[0], 0) < 0 || close(fd[0]) < 0|| close(fd[1]) < 0))
		exit_fatal();
	return (ret);
}

int main(int ac, char **av, char **env)
{
	int i = 0;
	int status = 0;
	if (ac > 1)
	{
		while(av[i])
		{
			av += i + 1;
			i = 0;
			while (av[i] && strcmp(av[i], "|") && strcmp(av[i], ";"))
				i++;
			if (i)
				status = exec_(av, i, env);
		}
	}
	return (status);
}
