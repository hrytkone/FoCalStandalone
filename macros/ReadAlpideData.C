const int npix = 2;
const int nalpide = 18;

vector<pair<int,float>> *data_pix[npix*nalpide];

void ReadAlpideData()
{
    gInterpreter->GenerateDictionary("vector<pair<int,float>>", "vector");

    TFile *fin = TFile::Open("hits.root", "READ");
    if (!fin) return;

    TTree *tree = (TTree*)fin->Get("DataTree");

    for (int i=0; i<npix; i++) {
        for (int j=0; j<nalpide; j++) {
          tree->SetBranchAddress(Form("data_pix%d_alpide%d", i, j), &data_pix[i*nalpide + j]);
        }
    }

    for (int i=0; i<tree->GetEntries(); i++) {
        cout << "\nEvent " << i+1 << endl;
        tree->GetEntry(i);

        for (int i=0; i<npix*nalpide; i++) {
            cout << "alpide " << i << "  size : " << data_pix[i]->size() << endl;
            for (int j=0; j<data_pix[i]->size(); j++) {
                pair<int,float> data = data_pix[i]->at(j);
                cout << "\tid : " << data.first << "   value : " <<data.second << endl;
            }
        }
    }
}
