void str_to_upper(char *s)
{
	for(int i = 0; s[i] != 0; i++)
	{
		if (s[i] >= 'a' && s[i] <= 'z')
		{
			s[i] -= 32;
		}
	}
}

int  str_count(const char *s)
{
	int count = 0;
	for (int i = 0; s[i] != 0; i++)
	{
		count++;
	}
	return	count;
}

void str_reverse(char *s)
{
	int i = 0;
	int k = str_count(s) - 1;
	char temp;

	while(i < k)
	{
		temp = s[k];
		s[k] = 	s[i];
		s[i] = temp;

		i++;
		k--;
	}
}