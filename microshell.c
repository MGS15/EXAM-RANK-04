#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int	err(char *e1, char *e2)
{
	while (*e1)
		write(2, e1++, 1);
	if (!e2)
		return (1);
	while (*e2)
		write(2, e2++, 1);
	return (write(2, "\n", 1));
}

void	ft_cd(char **cmd, int i)
{
	if (i != 2)
	{
		err("error: cd: bad arguments\n", NULL);
		return ;
	}
	if (chdir(cmd[1]))
		err("error: cd: cannot change directory to ", cmd[1]);
}

#include <stdio.h>

void	execute(char **cmd, int i, char **env)
{
	cmd[i] = NULL;
	execve(cmd[0], cmd, env);
	err("error: cannot execute ", cmd[0]);
}

int	main(int c, char **v, char **env)
{
	int	i;
	int	pid;
	int	tfd;
	int	fd[2];

	if  (c < 2)
		return (1);
	i = 0;
	while (v[i])
	{
		v = v + i + 1;
		i = 0;
		while (v[i] && strcmp(v[i], "|") && strcmp(v[i], ";"))
			i++;
		if (i && !strcmp(v[0], "cd"))
			ft_cd(v, i);
		else if (i && (!v[i] || !strcmp(v[i], ";")))
		{
			pid = fork();
			if (pid == -1)
				return (err("error: fatal\n", NULL), 1);
			else if (!pid)
				execute(v, i, env);
			else
				while (waitpid(pid, NULL, 0) != -1);
		}
		else if (v[i] && !strcmp(v[i], "|"))
		{
			tfd = dup(0);
			if (pipe(fd) == -1)
				return (err("error: fatal\n", NULL), 1);
			if (dup2(fd[0], tfd) == -1)
				return (err("error: fatal\n", NULL), 1);
			pid = fork();
			if (pid == -1)
				return (err("error: fatal\n", NULL), 1);
			else if (!pid)
			{
				if (dup2(fd[1], 1) == -1)
					return (err("error: fatal\n", NULL), 1);
				close(fd[0]);
				close(fd[1]);
				execute(v, i, env);
			}
			else
			{
				if (dup2(tfd, 0) == -1)
					return (err("error: fatal\n", NULL), 1);
				close(fd[0]);
				close(fd[1]);
				close(tfd);
			}
		}
	}
}
