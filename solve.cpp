#include<iostream>
#include<fstream>
#include<set>
#include<map>
using namespace std;

class SudokuFrame{
    private:
    int frame[9][9];
    bool isEditable[9][9];
    public:
    SudokuFrame(){
        menu();
    }
    private:void menu(){
        cout<<"\n======================\n";
		cout<<"    Sudoku Solver\n";
		cout<<"======================\n\n";

        cout << "INPUT the puzzle\n";
        cout << "Enter 1 for giving input through console\n";
        cout << "Enter 2 for to read input from a file\n";

        int input;
        cin >> input;

        if(input==1) readThruConsole();
        else if(input==2)readThruFile();
        else{
            int count=0;
            while(count < 3){
                cout << "\nWrong Input , Try again (chances left-3)\n";
                count+=2;
                cin >> input;
                if(input==1) readThruConsole();
                else if(input==2)readThruFile();
                else continue;

                break;
            }
        }
    }
    private:void readThruConsole(){
        cout << "\nEnter values row by row\n";
        cout << "Input 0 ,in case the cell is empty\n";
        
        int row=0,col=0;
        for(row=0;row<9;row++){
            for(col=0;col<9;col++){
                int value;
                cout << "Enter value of cell [" << row+1 << "," << col+1 << "]-->";
                cin >> value;
                if(!(value>=0 && value<=9)){
                    while(1){
                        cout << "Value out of bound ! Try again\n";
                        cin >> value;
                        if(value>=0 && value<=9) break;
                    }
                }
                frame[row][col]=value;
                if(value==0) isEditable[row][col]=true;
                else isEditable[row][col]=false;
            }
        }
        cout << "--------\n";
    }
    private:void readThruFile(){
        string filename;
        cout << "\nEnter the file name that contains Sudoku puzzle\n";
        cout << "\t  --> ";
        cin >> filename;

        ifstream sudoku;
        sudoku.open(filename,ios::in);

        int row,col;
        for(row=0;row<9;row++){
            for(col=0;col<9;col++){
                int value;
                sudoku >> value;
                if(!(value>=0 && value<=9)){
                    cout << "\nValue " << (row*9)+col+1 << "is out of bound !\n";
                    exit(EXIT_FAILURE);
                }  
                frame[row][col]=value;
                if(value==0) isEditable[row][col]=true;
                else isEditable[row][col]=false;

                if(sudoku.eof()) break;   
            }
        }
        sudoku.close();
        cout << "--------\n";
    }
    public:void setvalue(int row,int col,int num){
        if(isEditable[row][col])frame[row][col]=num;
    }
    public:int getvalue(int row,int col){
        return frame[row][col];
    }
    public:void display(){
        cout<<"++============================++";
        int row,col;
        for(row=0;row<9;row++){
            cout << "\n||";
            int cell=1;
            for(col=0;col<9;col++){
                if(cell==3){
                    cout << frame[row][col] << " ";
                    cout << "||" ;
                    cell=1;
                }
                else{
                    cout << frame[row][col] << "  ";
                    cell++;
                }
            }
            if(row%3!=2) cout<<"\n++--------++--------++--------++";
            else   cout<<"\n++============================++"; 
        }
    }
    public:bool editable(int row,int col){
        return isEditable[row][col];
    }
    public:void clear_frame(int row,int col){
        bool flag=1;
        for(int i=row;i<9;i++){
            int j;
            if(flag){
                j=col;
                flag=0;
            }else j=0;
            for(;j<9;j++){
                if(isEditable[i][j]==1) frame[i][j]=0;    
            }
        }
    }
};

class candidate{
    struct node{
        int data;
        node*next;
    };
    node*head;
    node*tail;
    node*ptr;
    public:candidate(){
        head=new struct node;
        head->next=0;
    }
    public:~candidate(){
        ptr=head;
        while(ptr!=0){
            node*temp=ptr;
            ptr=ptr->next;
            delete temp;
        }
    }
    public:int length(){
        int len=0;
        ptr=head->next;
        while(ptr!=0){
            len++;
            ptr=ptr->next;
        }
        return len; 
    }
    public:void print(){
        ptr=head->next;
        while(ptr!=0){
            cout << ptr->data << ",";
            ptr=ptr->next;
        }
        cout << "\n";
    }
    public:void append(int d){
        node*new_node=new node;
        new_node->data=d;
        new_node->next=0;

        if(!head->next){
            head->next=new_node;
            tail=head->next;
            return ;
        }
        tail->next=new_node;
        tail=new_node;
    }
    public:int operator[](int i){
        int count=0;
        ptr=head->next;
        while(ptr!=0){
            if(count==i)
                return ptr->data;
            ptr=ptr->next;
            count++;
        }
        return -1;
    }
    public:void copy(candidate cand){
        int oldLen=cand.length();
        ptr=head;
        for(int i=0;i<oldLen;i++){
            node *temp=new node;
            temp->next=0;
            temp->data=cand[i];

            ptr->next=temp;
            ptr=ptr->next;
        }
    }
};

class SudokuSolver{
    int backTrackingCount;
    SudokuFrame sudokuframe;

    // map<int,set<int>>rowChecker;
    // map<int,set<int>>colChecker;
    // map<pair<int,int>,set<int>>squares;
    public:SudokuSolver(){
        backTrackingCount=0;

		cout<<"\nCalculating possibilities...\n";
		cout<<"Backtracking across puzzle....\n";
		cout<<"Validating cells and values...\n\n";
		
		solve();
		cout<<"QED. Your puzzle has been solved!\n\n";
		sudokuframe.display();
        printStat();
		cout<<"\n\n";
        
	}
    private:bool valid_value(int row,int col,int num){
        for(int i=0;i<9;i++){
            if(i!=row){
                int dup=sudokuframe.getvalue(i,col);
                if(dup==num) return false;
            }
        }
        for(int j=0;j<9;j++){
            if(j!=col){
                int dup=sudokuframe.getvalue(row,j);
                if(dup==num) return false;
            }
        }
        int srow=(row/3)*3;
        int erow=srow+2;

        int scol=(col/3)*3;
        int ecol=scol+2;

        for(int i=srow;i<=erow;i++){
            for(int j=scol;j<=ecol;j++){
                if(sudokuframe.getvalue(i,j)==num) return false;
            }
        }
        return true;
    }
    private:candidate possible_values(int row,int col){
        candidate cand;
        for(int i=1;i<=9;i++){
            if(valid_value(row,col,i)==1){
                cand.append(i);
            }
        }
        return cand;
    }
    private:bool SingleCellSolver(int row,int col){
        statsincr();

        if(sudokuframe.editable(row,col)){
            candidate cand;
            cand.copy(possible_values(row,col));
            int l=cand.length();
            int next_row=row,next_col=col;
            for(int i=0;i<l;i++){
                int pvalue=cand[i];
                sudokuframe.setvalue(row,col,pvalue);

                if(col<8)next_col=col+1;
                else if(col==8){
                    if(row==8) return 1;
                    next_row=row+1;
                    next_col=0;
                }
                if(SingleCellSolver(next_row,next_col)==0){
                    sudokuframe.clear_frame(next_row,next_col);
                }
                else return 1;
            }
            return 0;
        }else{
            int next_row=row,next_col=col;
            if(col<8)next_col=col+1;
            else if(col==8){
                    if(row==8) return 1;
                    next_row=row+1;
                    next_col=0;
            }
            return SingleCellSolver(next_row,next_col);
        }
    }
    private:void solve(){
        int success=SingleCellSolver(0,0);
    }
    private:void displayFrame(){
        sudokuframe.display();
        printStat();
    }
    private:void statsincr(){
        backTrackingCount+=1;
    }
    private:void printStat(){
        cout << "\n\nSingleCellSolver() was called for " << backTrackingCount << " times." << endl;
    }
};

int main(){
    SudokuSolver ss;
    return 0;
}