# MarkDown Doc

Here you can find actual examples of elements that can be used from the [MarkDown Converter](https://github.com/MarkusHarnusek/MarkDownConverter)
Like a special element? Check out the signature in `introduction.md` or visit the [MarkDown Converter GitHub Repository](https://github.com/MarkusHarnusek/MarkDownConverter)

## Examples of Markdown Elements

### Headings

# Heading 1
## Heading 2
### Heading 3

### Lists
- Unordered list item 1
- Unordered list item 2
1. Ordered list item 1
2. Ordered list item 2

### Links and Images
[Visit GitHub](https://github.com)
![Sample Image](https://images.unsplash.com/photo-1683322499436-f4383dd59f5a)(350)

### Code Blocks
```
function greet() {
    console.log("Hello, World!");
}
```

### Tables
| Name     | Age | Occupation  |
|----------|-----|-------------|
| John Doe | 30  | Developer   |
| Jane Doe | 28  | Designer    |

### Special Items
#### Positive Signature Block
#+h Positive Heading  
#+b This is a positive message.

#### Warning Signature Block
#wh Warning Heading  
#wb This is a warning message.

#### Negative Signature Block
#-h Negative Heading  
#-b This is a negative message.

#### Info Signature Block
#ih Info Heading  
#ib This is an informational message.

#### Sources
#src[MarkDown Converter](https://github.com/MarkusHarnusek/MarkDownConverter)
#src[MarkDown Doc](https://github.com/MarkusHarnusek/MarkDownDoc)

#### Calculations
#cal
√(a² + b²) * log(c + 1) / (1 + e^(−d))
#cal
#### Formatted code blocks
#codefs(C)
#include <stdio.h>
#include <math.h>

#define MAX_VALUES 10

typedef struct {
    double values[MAX_VALUES];
    int count;
} DataSet;

double calculate_mean(DataSet *data) {
    double sum = 0.0;
    for (int i = 0; i < data->count; i++) {
        sum += data->values[i];
    }
    return sum / data->count;
}

double calculate_stddev(DataSet *data, double mean) {
    double variance = 0.0;
    for (int i = 0; i < data->count; i++) {
        double diff = data->values[i] - mean;
        variance += diff * diff;
    }
    return sqrt(variance / data->count);
}

int main(void) {
    DataSet data = {{12.5, 15.0, 18.2, 20.1, 22.8}, 5};

    double mean = calculate_mean(&data);
    double stddev = calculate_stddev(&data, mean);

    printf("Data count: %d\n", data.count);
    printf("Mean value: %.2f\n", mean);
    printf("Standard deviation: %.2f\n", stddev);

    return 0;
}
#codefe

#### End Tag
This version of the MarkDown Converter also supports a custom `#end` tag for switching to the previous and next page:

#end