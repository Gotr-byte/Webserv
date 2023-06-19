#include "../includes/ft_strchr.hpp"

char	*ft_strchr(const char *s, int c)
{
	size_t	i;
	char	*h;
	size_t	t;

	t = ft_strlen(s);
	h = (char *)s;
	i = 0;
	while (i <= t)
	{
		if (*(h + i) == (char)c)
			return (h + i);
		i++;
	}
	return (0);
}