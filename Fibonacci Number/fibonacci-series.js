<script>

// Javascript program for Fibonacci Series 
// using Space Optimized Method

function fib(n)
{
    let a = 0, b = 1, c, i;
    if( n == 0)
        return a;
    for(i = 2; i <= n; i++)
    {
    c = a + b;
    a = b;
    b = c;
    }
    return b;
}

// Driver code

    let n = 9;
    
    document.write(fib(n));


</script>
