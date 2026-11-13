#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <math.h>
#include <float.h>

#include "arg_parser.h"

static double *kolmogorov_coeff_matrix(
	int n,
	arg_list_t *mues,
	arg_list_t *lambdas
)
{
	int i, j;

	arg_list_t *mu;
	arg_list_t *lambda;

	double *matrix = calloc(n * n, sizeof(*matrix));
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			mu = mues;
			while (mu) {
				if (i == j - mu->number) {
					matrix[i * n + j] += mu->value;
				} else if (j == i - mu->number) {
					matrix[i * n + i] -= mu->value;
				}

				mu = mu->next;
			}

			lambda = lambdas;
			while (lambda) {
				if (j == i - lambda->number) {
					matrix[i * n + j] += lambda->value;
				} else if (i == j - lambda->number) {
					matrix[i * n + i] -= lambda->value;
				}

				lambda = lambda->next;
			}
		}
	}

	/* normalization */
	i = n - 1;
	for (j = 0; j < n; j++) {
		matrix[i * n + j] = 1.0;
	}

	return matrix;
}

static double matrix_norm(int n, const double *A)
{
	double max;
	int i;

	if (n > 0) {
		max = A[0];
	}
	for (i = 1; i < n; i++) {
		if (A[i] > max) {
			max = A[i];
		}
	}

	return max;
}

static void gauss_solve(int n, double *A, double *b, double *x)
{
	int i, j, k;

	double max;
	int pivot;

	double m;

	double eps;

	eps = n * DBL_EPSILON * matrix_norm(n, A);

	/* forward elimination */
	for (i = 0; i < n - 1; i++) {
		/* partial pivoting */
		max = 0.0;
		pivot = -1;
		for (j = i; j < n; j++) {
			if (fabs(A[j * n + i]) - max > eps) {
				max = fabs(A[j * n + i]);
				pivot = j;
			}
		}
		/* skip if no pivot element */
		if (pivot == -1) {
			continue;
		}
		/* pivoting: row swap */
		memmove(&A[i * n], &A[pivot * n], n);

		for (j = i + 1; j < n; j++) {
			if (fabs(A[j * n + i] - 0.0) < eps) {
				continue;
			}
			/* elimination multiplier */
			m = A[j * n + i] / A[i * n + i];

			/* elimination step */
			for (k = 0; k < n; k++) {
				A[j * n + k] -= m * A[i * n + k]; 
			}
			b[j] -= m * b[i];
		}
	}

	/* back substitution */
	for (i = n - 1; i >= 0; i--) {
		x[i] = 0.0 / 0.0; /* NaN */

		for (j = n - 1; j > i; j--) {
			b[i] -= A[i * n + j] * x[j];
			A[i * n + j] = 0.0;
		}
		if (fabs(A[i * n + i] - 0.0) > eps) {
			x[i] = b[i] / A[i * n + i];
			A[i * n + i] = 1.0;
		}
	}
}

typedef struct svg_render
{
	FILE *context;
} svg_render_t;

static arg_list_t *arg_list_next_min(arg_list_t *list, int last_number)
{
	arg_list_t *arg;
	arg_list_t *best;
	int min_number;

	best = NULL;
	min_number = -1;

	arg = list;
	while (arg) {
		if (arg->number > last_number) {
			if (min_number == -1 || arg->number < min_number) {
				min_number = arg->number;
				best = arg;
			}
		}
		arg = arg->next;
	}

	return best;
}

static void svg_render_label(
	svg_render_t *render,
	double x,
	double y,
	const char *label,
	int index
)
{
	fprintf(
		render->context,
		"<text"
		" x=\"%f\""
		" y=\"%f\""
		" text-anchor=\"middle\""
		" dominant-baseline=\"central\">"
		" %s<tspan dy=\"0.3em\" font-size=\"0.7em\">%d</tspan>"
		"</text>\n",
		x, y,
		label, index
	);
}

static void svg_render_line(
        svg_render_t *render,
        double x1,
        double y1,
        double x2,
        double y2,
        const char *marker_end
)
{
        fprintf(
                render->context,
                "<line"
                " x1=\"%f\""
                " y1=\"%f\""
                " x2=\"%f\""
                " y2=\"%f\""
                " marker-end=\"url(#%s)\""
                "/>\n",
                x1, y1,
                x2, y2,
                marker_end
        );
}

static void svg_render_sline(
	svg_render_t *render,
	double x1,
	double y1,
	double x2,
	double y2,
	double x3,
	double y3,
	const char *marker_end
)
{
	fprintf(
		render->context,
		"<path d=\"M %f %f Q %f %f %f %f\""
		" fill=\"none\""
      		" marker-end=\"url(#%s)\"/>",
		x1, y1,
		x2, y2,
		x3, y3,
		marker_end
	);
}

static void svg_render_short_edge(
	svg_render_t *render,
	double x,
	double y,
	double r,
	double shift,
	double sign,
	const char *symbol
)
{
	double x1, y1;
	double x2, y2;

	x1 = x + shift - cos(asin(1.0 / 3.0)) * r;
	y1 = y + sign * ((1.0 / 3.0) * r);

	x2 = x + cos(asin(1.0 / 3.0)) * r;
	y2 = y1;

	svg_render_line(render, x1, y1, x2, y2, "arrow");
	svg_render_label(
		render,
		x1 + (x2 - x1) / 2.0,
		y1 + 0.1 * r,
		symbol,
		1
	);
}

static void svg_render_long_edge(
	svg_render_t *render,
	double x,
	double y,
	double r,
	double shift,
	double sign,
	int number,
	int j,
	const char *symbol,
	double *extreme
)
{
	double x1, y1;
	double x2, y2;
	double x3, y3;

	if (sign > 0) {
		/* lambda: edge down */
		x1 = x;
		y1 = y + r;

		x2 = x + (shift * number) / 2.0;
		y2 = y + 2.0 * r * (j + 2);

		x3 = x + (shift * number);
		y3 = y1;
	} else {
		/* mu: edge up */
		x1 = x + (shift * number);
		y1 = y - r;

		x2 = x + (shift * number) / 2.0;
		y2 = y - 2.0 * r * (j + 2);

		x3 = x;
		y3 = y1;
	}

	svg_render_sline(render, x1, y1, x2, y2, x3, y3, "arrow");
	svg_render_label(
		render,
		x2,
		0.5 * y1 + 0.5 * y2 + 0.1 * r,
		symbol,
		number
	);

	*extreme = 0.5 * y1 + 0.5 * y2 + 0.1 * r;
}

static void svg_render_edges(
	svg_render_t *render,
	arg_list_t *list,
	int i,
	int n,
	double x,
	double y,
	double r,
	double shift,
	double sign,
	const char *symbol,
	double *extreme
)
{
	arg_list_t *edge;
	int last_number;
	int j;

	last_number = -1;
	edge = list;
	while (edge) {
		if (edge->number == 1) {
			if (i < n - 1) {
				svg_render_short_edge(
					render, x, y, r, shift, sign, symbol
				);
				*extreme = r;
			}
			last_number = edge->number;
			break;
		}
		edge = edge->next;
	}

	for (j = 0; j < n; j++) {
		edge = arg_list_next_min(list, last_number);
		if (edge == NULL) {
			break;
		}
		last_number = edge->number;

		if (i + edge->number >= n) {
			continue;
		}

		svg_render_long_edge(
			render, x, y, r, shift, sign,
			edge->number, j, symbol, extreme
		);
	}
}

static void svg_render_defs(FILE *body)
{
	fprintf(body,
		"<defs>"
		"  <marker id=\"arrow\""
		"   viewBox=\"0 0 10 10\""
		"   refX=\"10\""
		"   refY=\"5\""
		"   markerWidth=\"6\""
		"   markerHeight=\"6\""
		"   orient=\"auto\">"
		"    <path d=\"M 0 0 L 10 5 L 0 10 z\""
		"     fill=\"black\" />"
		"  </marker>"
		"</defs>"
	);
}

static void svg_render_circle(
	svg_render_t *render,
	double x,
	double y,
	double r,
	int index
)
{
	fprintf(
		render->context,
		"<circle"
		" cx=\"%f\""
		" cy=\"%f\""
		" r=\"%f\""
		" fill=\"none\""
		" stroke=\"black\""
		"/>\n",
		x, y, r
	);
	svg_render_label(render, x, y, "S", index);
}

static void svg_write_header(FILE *file)
{
	fprintf(
		file,
		"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
	);

	fprintf(
		file,
		"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n"
		"  \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n"
	);
}

static void svg_write_root_open(
	FILE *file,
	double minX,
	double minY,
	double width,
	double height
)
{
	fprintf(
		file,
		"<svg version=\"1.1\"\n"
		" baseProfile=\"full\"\n"
		" xmlns=\"http://www.w3.org/2000/svg\"\n"
		" xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
		" xmlns:ev=\"http://www.w3.org/2001/xml-events\"\n"
		" width=\"100%%\" height=\"100%%\"\n"
		" stroke=\"black\"\n"
		" stroke-width=\"1\"\n"
		" viewBox=\"%.2f %.2f %.2f %.2f\">\n",
		minX, minY, width, height
	);
}

static void svg_copy_stream(FILE *dst, FILE *src)
{
	char chunk[4096];
	int k;

	rewind(src);
	while ((k = (int)fread(chunk, 1, sizeof chunk, src)) > 0) {
		fwrite(chunk, 1, (size_t)k, dst);
	}
}

static void render_svg(int n, arg_list_t *mues, arg_list_t *lambdas)
{
	svg_render_t render;
	FILE *body;
	FILE *file;
	double r;
	double x;
	double y;
	double shift;
	double minY;
	double maxY;
	double width;
	double height;
	int i;

	if (n <= 0) {
		return;
	}

	body = tmpfile();
	if (body == NULL) {
		return;
	}
	render.context = body;

	svg_render_defs(body);

	r = 50.0;
	x = r;
	y = r;
	shift = 4.0 * r;
	minY = r;
	maxY = r;

	for (i = 0; i < n; i++) {
		svg_render_circle(&render, x, y, r, i + 1);

		svg_render_edges(
			&render, lambdas, i, n,
			x, y, r, shift, +1.0, "&#955;", &maxY
		);
		svg_render_edges(
			&render, mues, i, n,
			x, y, r, shift, -1.0, "&#956;", &minY
		);

		x += shift;
	}
	width = x - shift + r;
	height = -minY + maxY;

	fprintf(body, "</svg>\n");

	file = fopen("graph.svg", "w");
	if (file == NULL) {
		fclose(body);
		return;
	}

	svg_write_header(file);
	svg_write_root_open(file, -r, minY - r, width + 2.0 * r, height + 2.0 * r);
	svg_copy_stream(file, body);

	fclose(body);
	fclose(file);
}

int main(int argc, char **argv)
{
	int result = 0;

	int argn;
	char *arg;
	char sym;

	arg_parser_t parser;

	int i;
	int n;

	arg_list_t *mues = 0;
	arg_list_t *lambdas = 0;

	double *A = 0;
	double *b = 0;
	double *x = 0;

	arg_parser_init(&parser);

	for (argn = 1; argn < argc; argn++) {
		arg = argv[argn];
		while (*arg != '\0') {
			sym = *arg;
			arg_parser_proc(&parser, sym);

			++arg;
		}
		if (parser.state == ARG_PARSER_STATE_ERROR) {
			result = 2;

			goto EXIT;
		}
		arg_parser_proc(&parser, ' ');
	}

	arg_parser_result(&parser, &mues, &lambdas, &n);
	if (n == -1) {
		result = 2;
		fprintf(
			stderr,
			"%s: number of states n is not specified (use 'n=<value>' syntax)\n",
			argv[0]
		);
	}
	if (!mues || !lambdas) {
		result = 2;

		goto EXIT;
	}

	A = kolmogorov_coeff_matrix(n, mues, lambdas);

	render_svg(n, mues, lambdas);

	b = calloc(n, sizeof(*b));
	b[n - 1] = 1.0;

	x = malloc(n * sizeof(*x));
	
	gauss_solve(n, A, b, x);

	for (i = 0; i < n - 1; i++) {
		printf("%f ", x[i]);
	}
	printf("%f\n", x[i]);

EXIT:
	arg_parser_free(&parser);

	arg_list_free(&mues);
	arg_list_free(&lambdas);

	free(A);
	free(b);
	free(x);

	return result;
}

