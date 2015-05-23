#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

FILE *stream;

struct item_price {
	int price;
	int index;
};

static int get_int_from_stdin(int *i) {
	int r;

	assert(i);

	r = scanf("%d", i);
	if (r != 1)
		return -errno;

	return r;
}

static int load_test_case(int *credit, int *num_items, int **prices) {
	int r, i, *p;

	assert(credit);
	assert(num_items);
	assert(prices);

	r = get_int_from_stdin(credit);
	if (r != 1)
		return -errno;

	r = get_int_from_stdin(num_items);
	if (r != 1)
		return -errno;

	p = calloc(*num_items, sizeof(*p));
	if (!p)
		return -ENOMEM;
	
	for (i = 0; i < *num_items; ++i) {
		int price;

		r = get_int_from_stdin(&price);
		if (r != 1) {
			free(p);
			return -errno;
		}

		p[i] = price;
	}
	
	*prices = p;

	return 0;
}

static int cmp_prices(const void *p1, const void *p2) {
	int left = ((const struct item_price *) p1)->price, right = ((const struct item_price *) p2)->price;

	return ((left > right) - (left < right));
}

static int cmp_price_credit(const void *key, const void *elem) {
	int price = ((const struct item_price *) elem)->price, credit = *(const int *) key;
	
	return credit < price ? -1 : credit > price ? 1 : 0;
}

static void solve(int case_num, int credit, int num_items, int *prices) {	
	int i;
	struct item_price *price_list;

	assert(prices);

	price_list = calloc(num_items, sizeof(*price_list));
	if (!price_list)
		return;

	for (i = 0; i < num_items; ++i) {
		price_list[i].price = prices[i];
		price_list[i].index = i;
	}

	qsort(price_list, num_items, sizeof(*price_list), cmp_prices);

	for (i = 0; i < num_items; ++i) {
		struct item_price *e;
		int c;

		c = credit - prices[i];
	
		e = bsearch(&c, price_list, num_items, sizeof(*price_list), cmp_price_credit);
		if (e && e->index != i) {
			assert(e->price + prices[i] == credit);
			
			fprintf(stream, "Case #%d: %d %d\n", case_num, i + 1, e->index + 1);
			fflush(stream);
			break;
		}
	}
}

int main(void) {
	int i, r, test_cases, credit, num_items, *prices = NULL;
	char *stream_buf = NULL;
	size_t size;

	stream = open_memstream(&stream_buf, &size);

	r = get_int_from_stdin(&test_cases);
	if (r < 0) {
		fprintf(stderr, "Failed to determine number of test cases\n");
		return EXIT_FAILURE;
	}

	for (i = 0; i < test_cases; ++i) { 
		free(prices);
		prices = NULL;

		r = load_test_case(&credit, &num_items, &prices);
		if (r < 0) {
			free(stream_buf);
			fprintf(stderr, "Failed to parse test cases from stdin\n");
			return EXIT_FAILURE;
		}

		solve(i + 1, credit, num_items, prices);
	}

	free(prices);

	printf("%s", stream_buf);
	free(stream_buf);
	
	return 0;
}
