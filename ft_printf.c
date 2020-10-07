#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <limits.h>

#define ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#define MAX(a, b) ((a > b) ? a : b)
#define CTOD(c) ((c) - '0')
#define DTOC(d) ((d) + '0')

int ft_putstr_shift(const char **fmt)
{
	const char *p;

	p = *fmt;
	while (**fmt && **fmt != '%')
		(*fmt)++;
	return (write(1, p, *fmt - p));
}

int ft_atoi_shift(const char **fmt)
{
	int i = 0;

	while (ISDIGIT(**fmt))
		i = i * 10 + CTOD(*(*fmt)++);
	return (i);
}

void ft_parse_width(const char **fmt, va_list ap, int *width)
{
	if (ISDIGIT(**fmt))
		*width = ft_atoi_shift(fmt);
	else if (**fmt == '*')
	{
		*width = va_arg(ap, int);
		if (*width < 0)
			*width = -*width;
		(*fmt)++;
	}
}

void ft_parse_prec(const char **fmt, va_list ap, int *prec)
{
	*prec = 0;
	++(*fmt);
	if (ISDIGIT(**fmt))
		*prec = ft_atoi_shift(fmt);
	else if (**fmt == '*')
	{
		*prec = va_arg(ap, int);
		if (*prec < 0)
			*prec = -1;
		(*fmt)++;
	}
}

int ft_parse_shift(const char **fmt, va_list ap, int *width, int *prec, int *type)
{
	++(*fmt);
	if (ISDIGIT(**fmt) || **fmt == '*')
		ft_parse_width(fmt, ap, width);
	if (**fmt == '.')
		ft_parse_prec(fmt, ap, prec);
	if (**fmt == 's' || **fmt == 'd' || **fmt == 'x')
		*type = *(*fmt)++;
	return (0);
}

char *ft_strndup(char *s, int n)
{
	char *res;
	int i = -1;

	if (!(res = malloc(sizeof(char) * (n + 1))))
		return (NULL);
	while (++i < n)
		res[i] = s[i];
	res[i] = '\0';
	return (res);
}

int ft_strlen(char *s)
{
	char *p;

	p = s;
	while (*p)
		p++;
	return (p - s);
}

int ft_strnlen(char *s, int maxlen)
{
	char *p;

	p = s;
	while (*p && maxlen)
	{
		p++;
		maxlen--;
	}
	return (p - s);
}

void ft_swap(char *a, char *b)
{
	char c;

	c = *a;
	*a = *b;
	*b = c;
}

void ft_reverse(char *s)
{
	int i;
	int j;

	i = 0;
	j = ft_strlen(s) - 1;
	while (i < j)
		ft_swap(&s[i++], &s[j--]);
}

char *ft_uitoa_base(unsigned int n, int base)
{
	const char digits[16] = "0123456789abcdef";
	char s[11];
	int i = 0;

	s[i++] = digits[n % base];
	while ((n /= base) > 0)
		s[i++] = digits[n % base];
	s[i] = '\0';
	ft_reverse(s);
	return (ft_strndup(s, i));
}

int ft_putnchar(char c, int n)
{
	int i = 0;

	while (i < n)
		i += write(1, &c, 1);
	return (i);
}

int ft_put_free(int width, int sign, int prec, char *value)
{
	int n = 0;
	int len = 0;

	len = ft_strlen(value);
	n += ft_putnchar(' ', width - sign - prec);
	n += ft_putnchar('-', sign);
	n += ft_putnchar('0', prec - len);
	n += write(1, value, len);
	free(value);
	return (n);
}

int ft_puts(int width, int prec, char *s)
{
	char *value;
	int len;

	s = s ? s : "(null)";
	len = ft_strnlen(s, prec);
	value = ft_strndup(s, len);
	if (!value)
		return (-1);
	return (ft_put_free(width, 0, len, value));
}

int ft_putd(int width, int prec, long d)
{
	char *value;
	int sign = 0;

	if (d < 0) {
		d = -d;
		sign = 1;
	}
	value = ft_uitoa_base(d, 10);
	if (!value)
		return (-1);
	if (d == 0 && prec == 0)
		*value = '\0';
	prec = MAX(ft_strlen(value), prec);
	return (ft_put_free(width, sign, prec, value));
}

int ft_putx(int width, int prec, unsigned int x)
{
	char *value;

	value = ft_uitoa_base(x, 16);
	if (!value)
		return (-1);
	if (x == 0 && prec == 0)
		*value = '\0';
	prec = MAX(ft_strlen(value), prec);
	return (ft_put_free(width, 0, prec, value));
}

int ft_putspec_shift(const char **fmt, va_list ap)
{
	int width, prec, type;

	width = -1, prec = -1, type = 0;
	if (ft_parse_shift(fmt, ap, &width, &prec, &type) == -1)
		return (-1);
	if ((char)type == 's')
		return (ft_puts(width, prec, va_arg(ap, char *)));
	if ((char)type == 'd')
		return (ft_putd(width, prec, va_arg(ap, int)));
	if ((char)type == 'x')
		return (ft_putx(width, prec, va_arg(ap, unsigned int)));
	return (0);
}

int ft_printf(const char *fmt, ...)
{
	va_list ap;
	int total = 0;
	int n = 0;

	va_start(ap, fmt);
	while (*fmt)
	{
		if (*fmt != '%')
			total += ft_putstr_shift(&fmt);
		else
		{
			n = ft_putspec_shift(&fmt, ap);
			if (n == -1)
			{
				total = -1;
				break ;
			}
			total += n;
		}
	}
	va_end(ap);
	return (total);
}

#ifdef DEBUG
int main(void)
{
	char *s = "hello, world";
	printf(" (%d)\n", printf("stay awhile"));
	printf(" (%d)\n", printf("%2s, %.s, %2.4s, %8.4s, %.*s, %.0s, %.1s", NULL, NULL, NULL, NULL, -2, NULL, NULL, NULL));
	printf(" (%d)\n", printf("you %s, %.s, %8.3s, %10s, %1.0s %1s, %5.15s, %15.5s, %1.1s are", "", "", "", s, s, s, s, s, s));

	printf(" (%d)\n", printf("you %d, %.d, %2.3d, %10d, %1.1d, %5.15d are", 0, 0, 0, 0, 0, 0));
	printf(" (%d)\n", printf("you %d, %.d, %8.3d, %10d, %1.1d, %5.15d", INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX));
	printf(" (%d)\n", printf("you %d, %.d, %8.3d, %10d, %1.1d, %5.15d", INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN));
	printf(" (%d)\n", printf("you %d, %.d, %8.3d, %10d, %1.1d, %5.15d", 1, 12345, 12345, -12345, -12345, 123456789));

	printf(" (%d)\n", printf("you %x, %.x, %2.3x, %10x, %1.1x, %5.15x are", 0, 0, 0, 0, 0, 0));
	printf(" (%d)\n", printf("you %x, %.x, %8.3x, %10x, %1.1x, %5.15x", INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX));
	printf(" (%d)\n", printf("you %x, %.x, %8.3x, %10x, %1.1x, %5.15x", INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN));
	printf(" (%d)\n", printf("you %x, %.x, %8.3x, %10x, %1.1x, %5.15x", 1, 12345, 12345, 1028, 64, 123456789));
	return (0);
}
#endif

#ifdef FT_DEBUG
int main(void)
{
	char *s = "hello, world";
	ft_printf(" (%d)\n", ft_printf("stay awhile"));
	ft_printf(" (%d)\n", ft_printf("%2s, %.s, %2.4s, %8.4s, %.*s, %.0s, %.1s", NULL, NULL, NULL, NULL, -2, NULL, NULL, NULL));
	ft_printf(" (%d)\n", ft_printf("you %s, %.s, %8.3s, %10s, %1.0s %1s, %5.15s, %15.5s, %1.1s are", "", "", "", s, s, s, s, s, s));

	ft_printf(" (%d)\n", ft_printf("you %d, %.d, %2.3d, %10d, %1.1d, %5.15d are", 0, 0, 0, 0, 0, 0));
	ft_printf(" (%d)\n", ft_printf("you %d, %.d, %8.3d, %10d, %1.1d, %5.15d", INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX));
	ft_printf(" (%d)\n", ft_printf("you %d, %.d, %8.3d, %10d, %1.1d, %5.15d", INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN));
	ft_printf(" (%d)\n", ft_printf("you %d, %.d, %8.3d, %10d, %1.1d, %5.15d", 1, 12345, 12345, -12345, -12345, 123456789));

	ft_printf(" (%d)\n", ft_printf("you %x, %.x, %2.3x, %10x, %1.1x, %5.15x are", 0, 0, 0, 0, 0, 0));
	ft_printf(" (%d)\n", ft_printf("you %x, %.x, %8.3x, %10x, %1.1x, %5.15x", INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX));
	ft_printf(" (%d)\n", ft_printf("you %x, %.x, %8.3x, %10x, %1.1x, %5.15x", INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN, INT_MIN));
	ft_printf(" (%d)\n", ft_printf("you %x, %.x, %8.3x, %10x, %1.1x, %5.15x", 1, 12345, 12345, 1028, 64, 123456789));
	return (0);
}
#endif
