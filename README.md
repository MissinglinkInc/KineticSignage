KineticSignage
==============

## KinectServer
Kinect�̃T�[�o�BSkeletal Viewer�T���v���ɂ�����Ƃ����v�Z��WinSock��g�ݍ��񂾂��́B���܂�񓯊��Ƃ��C�ɂ������Ȃ��̂ŁASkeltal���m���\�b�h�̂Ȃ���UDP���Ԃ������Ă���B
2.5GHz���x��C2D��30%�����̂�i5���炢�������B��������300MB���炢�H���̂ōň�4GB���炢�ς�łȂ��Ƃ�΂��B
UDP�|�[�g65487�ԁi65535-AKB48�Ƃ����Ӗ����j

�E�肪�E�Ђ�����ɂ���������͍��肪���Ђ�����ɂ���ꍇ�A
�p�x=arctan(���y-�Ђ�y,���x-�Ђ�x)
�i�E��D��j

�v�Z������A�p�x��UDP�œ���l�b�g���[�N���Ƀu���[�h�L���X�g����B
����ɂ��A����Kinect���甭������f�[�^�𕡐��@��ŋ��L�\�B

## KineticSignage
�T�[�o���瑗����p�P�b�g�����ĕ`�悷��f�W�^���T�C�l�[�W�N���C�A���g�B���܂胁�����͕s�v�B
�����I�iblocking�j�Ȏ�M�����ƕ`�敔�ŃX���b�h�𕪂��Ă���B�O���[�o���ϐ��Ŋp�x�l�����L�B�iread/write�����m�ɕ�����Ă���̂ŁAqueue��mutex�������K�v���Ȃ��B���Ԃ�j

�`���Direct2D�B�n�[�h�R�[�h���ꂽ�f�B���N�g���̎ʐ^�����ׂēǂ݂ɍs���B
KinectServer���甭������p�x�������ƂɁA�ʐ^��؂�ւ��Ă����B