#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int	err(char *e1, char *e2)
{
	while (*e1)
		write(2, e1++, 1);
	if (e2)
		while (*e2)
			write(2, e2++, 1);
	return (write(2, "\n", 1));
}

int	ft_cd(char **cmd, int i)
{
	if (i != 2)
		return(err("error: cd: bad arguments", NULL));
	if (chdir(cmd[1]))
		return (err("error: cd: cannot change directory to ", cmd[1]));
	return (0);
}

int	ft_executer(char **cmd, int i, char **env, int tfd)
{
	cmd[i] = NULL;
	if (dup2(tfd, 0) == -1)
		return (err("error: fatal", NULL));
	close(tfd);
	execve(cmd[0], cmd, env);
	return (err("error: cannot execute ", cmd[0]));
}

int	main(int c, char **v)
{
	int	i;
	int	pid;
	int	tfd;
	int	fd[2];
	extern char	**environ;
	
	if (c < 2)
		return (1);
	i = 0;
	tfd = dup(0);
	if (tfd == -1)
		return (err("error: fatal", NULL));
	while (v[i] && v[i + 1])
	{
		v = v + 1 + i;
		i = 0;
		while (v[i] && strcmp(v[i], "|") && strcmp(v[i], ";"))
			i++;
		if (!strcmp(v[0], "cd"))
			ft_cd(v, i);
		else if (i && (!v[i] || !strcmp(v[i], ";")))
		{
			pid = fork();
			if (pid == -1)
				return (err("error: fatal", NULL));
			else if (!pid)
			{
				if (ft_executer(v, i, environ, tfd))
					return (1);
			}
			else
			{
				close(tfd);
				while (waitpid(-1, NULL, 0) != -1);
			}
			tfd = dup(0);
			if (tfd == -1)
				return (err("error: fatal", NULL));
		}
		else if (i && !strcmp(v[i], "|"))
		{
			if (pipe(fd) == -1)
				return (err("error: fatal", NULL));
			pid = fork();
			if (pid == -1)
				return (err("error: fatal", NULL));
			else if (!pid)
			{
				if (dup2(fd[1], 1) == -1)
					return (err("error: fatal", NULL));
				close(fd[0]);
				close(fd[1]);
				if (ft_executer(v, i, environ, tfd))
					return (1);
			}
			else
			{
				close(fd[1]);
				close(tfd);
				tfd = fd[0];
			}
		}
	}
	close(tfd);
	return (0);
}
