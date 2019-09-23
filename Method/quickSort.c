void increasingQuickSort(int** array, int column, int leftIndex, int rightIndex)
{
    // allow to sort in increasing order array allong a column

    if(leftIndex >= rightIndex) return;

    // pivot choice
    int pivot= array[rightIndex][column];

    // pointer
    int pointer = leftIndex;

    for(int i=leftIndex; i <= rightIndex; i++)
    {
        if(array[i][column] <= pivot)
        {
            //swap pointed and the one >= to the pivot
            int* memory = array[i];
            array[i] = array[pointer];
            array[pointer] = memory;
            pointer++;
        }
    }
    increasingQuickSort(array, column, leftIndex, pointer-2);
    increasingQuickSort(array, column, pointer, rightIndex);
}
