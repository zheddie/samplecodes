int maxLength(int* arr, int arrLen ) {
    // write code here
    int idx = 1;
    int idxStart=0;
    int maxlen = arrLen>0?1:0;

    while (idx<arrLen){
        for(int i =0;i<idx-idxStart;i++){
            if(arr[idxStart+i] == arr[idx] ){
                //Found dup here.
                //if(maxlen < idx-idxStart) maxlen = idx-idxStart;
                idxStart = idx++;
                break;
            }
        }
        if(i==idx-idxStart){
            if(maxlen < i+1) maxlen = i+1;
            idx++;
        }
    }
    return(maxlen);
}