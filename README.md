**Referenced Paper** </br>
Integrating Semantic Knowledge into Lexical Embeddings Based on Information Content Measurement. </br>
Author: Hsin-Yang Wang, Wei-Yun Ma. </br>
EACL 2017 short paper. </br>

**Implementation** </br>
CKIP Team, IIS. AS. 2017. </br>

Overview
--------
This project provides the implementation of the paper - Integrating Semantic Knowledge into Lexical Embeddings Based on Information Content Measurement. </br>

Usage: word2vec_joint
---------------------
word2vec_joint could produce the embeddings trained by Word2Vec, RCM, Threshold, Frequency, and Entropy. </br>

**Parameter Description** </br>

Example:
```
./word2vec_joint -corpus ../../../Data/Corpus/NYT/NYT.noTag.train -dimension 300 -word2vec 1 -word2vec-thread 12 -rcm 0 -pretrain 0 -output-model Model/NYT_WORD2VEC_R </br>
```
| parameter | function |
|----------|---------|
| **-corpus** <*file*> |                --    Use text data from <*file*> to train the model |
| **-dimension** <*int*> |              --    Set the dimension of the vectors; default is 300 |
| **-min-count** <*int*> |              --    This will discard words that appear less than *int* times; default is 0 |
| **-negative** <*int*> |               --    Number of negative examples; default is 15 |
| **-word2vec** <*int*> |               --    Use Word2Vec to train the vectors |
| **-word2vec-alpha** <*float*> |       --    Set the starting learning rate; default is 0.025 (For Word2Vec) |
| **-word2vec-epoch** <*int*> |         --    Run more training epochs; default is 1 (For Word2Vec) |
| **-word2vec-subsample** <*float*> |   --    Set threshold for occurrence of words. |
| **-word2vec-thread** <*int*> |        --    Use <*int*> threads (For Word2Vec) |
| **-word2vec-window** <*int*> |        --    Set max skip length between words; default is 5 |
| **-rcm** <*int*> |                    --    Use RCM with resource to train the vectors |
| **-rcm-epoch** <*int*> |              --    Run more training epochs; default is 100 (For RCM) |
| **-rcm-lambda** <*float*> |           --    Set the starting learning rate; default is 0.01 (For RCM) |
| **-rcm-resource** <*file*> |          --    Use relation pairs from <*file*> to train the model (For RCM) |
| **-rcm-ic** <*file*> |                --    Use IC from <*file*> to define the entropy of words (Entropy method) |
| **-rcm-thread** <*int*> |             --    Use *int* threads (For RCM)  |
| **-rcm-method** <*int*> |             --    Set 0 to train Baseline, 1 for Threshold, 2 for Frequency, 3 for Entropy |
| **-rcm-threshold** <*int*> |          --    Only revise the word which freq. is under than <*int*> (Threshold method)  </br>
| **-pretrain** <*int*> |               --    Use pretrain model |
| **-pretrain-model** <*file*> |        --    Use <*file*> as pretrain model |
| **-output-model** <*file*> |          --    Use <*file*> to save the resulting vectors |

If you want to reproduce the work, please follow up with the bash file "Bash_Train.sh" attached in the folder </br>

For questions, comments and report programing bug, please contact wang@iis.sinica.edu.tw </br>
