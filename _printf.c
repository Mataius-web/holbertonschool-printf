#include "main.h"
#include <stdarg.h>
#include <unistd.h>


int print_number(int n)
{
	int count = 0;
	char c;

	if (n == -2147483648)
	{
		write(1, "-2147483648", 11);
		return 11;
	}

	if (n < 0)
	{
		write(1, "-", 1);
		count++;
		n = -n;
	}

	if (n / 10)
		count += print_number(n / 10);

	c = (n % 10) + '0';
	write(1, &c, 1);
	count++;

	return count;
}

/**
 * _printf - produces output according to a format
 * @format: format string
 * Return: number of characters printed
 */


int _printf(const char *format, ...)
{
	va_list args;
	int i = 0, count = 0;
	char *str;
	char c;

	if (format == NULL)
		return (-1);

	va_start(args, format);

	while (format[i])
	{
		if (format[i] == '%')
		{
			i++;
			if (format[i] == '\0')
			{
				va_end(args);
				return (-1);
			}

			if (format[i] == 'c')
			{
				c = va_arg(args, int);
				write(1, &c, 1);
				count++;
			}
			else if (format[i] == 's')
			{
				str = va_arg(args, char *);
				if (!str)
					str = "(null)";
				while (*str)
				{
					write(1, str, 1);
					str++;
					count++;
				}
			}
			else if (format[i] == '%')
			{
				write(1, "%", 1);
				count++;
			}
			else if (format[i] == 'd' || format[i] == 'i')
			{
				int num = va_arg(args, int);
				count += print_number(num);
			}
			else
			{
				write(1, "%", 1);
				write(1, &format[i], 1);
				count += 2;
			}
		}
		else
		{
			write(1, &format[i], 1);
			count++;
		}
		i++;
	}

	va_end(args);
	return (count);
}
