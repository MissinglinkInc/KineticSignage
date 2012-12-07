KineticSignage
==============

## KinectServer
### �T�v
Kinect�̃T�[�o�BSkeletal Viewer�T���v����WinSock��g�ݍ��񂾂��́B���܂�񓯊��Ƃ��C�ɂ������Ȃ��̂ŁASkeltal���m���\�b�h�̂Ȃ���UDP���Ԃ������Ă���B

### requirements
2.5GHz���x��C2D��30%�����̂�i5���炢�������B��������300MB���炢�H���̂ōň�4GB���炢�ς�łȂ��Ƃ�΂��B
Kinect SDK�ADirect2D��Windows 7�ȏ��v������Ǝv����B

### �d�l�ƒ���
���i����UDP�|�[�g65487�Ԃœ���l�b�g���[�N���Ƀu���[�h�L���X�g����B���m�l������l�̏ꍇ�A���Ԃ�100kbps���炢�B

UDP�͌��m���[�v���ƂɃp�P�b�g�𑗂�BUDP�Ȃ̂Ō���������\���͂���B���̏ꍇ�A���n��i�����u�j�ŏ��̌��������邪�A���̂ق��͂Ȃ��B���Ƃ��ΐl�ԏ��̌����A���i�̈ꕔ���̌����Ȃǂ͋N����Ȃ��B�i���̏ꍇ�́AUDP�w�b�_�̃`�F�b�N�T���ɂ��͂������Ǝv����j

UDP��TCP�̂悤�ɃV�[�P���X�ԍ���ACK���Ȃ����߁A�N���C�A���g���ł́A���n��ł̃p�P�b�g�����A���n��̑O��ɋC�Â����Ƃ͂ł��Ȃ��B���̂��߁A���v���O�����͏��K�͂�����I�ȃ��[�J���l�b�g���[�N�ɂ�����^�p��z�肵�Ă���B
��������Ă���R�[�h�ł̓u���[�h�L���X�g���邽�߁A���[�^�ɂ���ăZ�O�����g���������ꂽ�l�b�g���[�N�ł̉^�p�𐄏�����B�i�����̏ꍇ�A���[�^��UDP�u���[�h�L���X�g�p�P�b�g��j������j

### �v���g�R���͈ȉ��F
    [
      [
        [float x, float y, float z, float w]...
      ]...
    ]...
#### skeleton position
x,y,z,w��float�̍��W�f�[�^�ł���B���̌Œ蒷�̔z��͍��i���W�ł���B

cf. struct Vector4, NuiSensor.h, Kinect SDK

#### skeleton positions = human
���i���W�͕����܂߂���B�����NuiSensor.h�ŗ񋓂���Ă���NUI_SKELETON_POSITION_*�̃C���f�b�N�X�Ɠ����C���f�b�N�X���U���Ă���

cf. enum NUI_SKELETON_POSITION_INDEX, NuiSensor.h, Kinect SDK

#### humans
skeleton positions (�܂�human)�̔z��Ƃ��Ă�humans

cf. struct NUI_SKELETON_FRAME.SkeletonData

���̔z��R���e�i��MessagePack�̃V���A���C�U�ɂ���ăV���A���C�Y����A�ЂƂ�UDP�f�[�^�O�������ău���[�h�L���X�g�����B
UDP�y�C���[�h�͂��̈��ނ����ł���B�y�C���[�h��MessagePack�Ńf�V���A���C�Y����΁A�ǂ̂悤�Ȋ��ł���L�z�񂪓W�J�����B
����ɂ��A����Kinect���甭������f�[�^�𕡐��@��ŋ��L�\�B

## KineticSignage
### �T�v
�T�[�o���瑗����p�P�b�g�����ĕ`�悷��f�W�^���T�C�l�[�W�N���C�A���g�B���܂胁�����͕s�v�BDirect2D�Ȃ̂�Windows 7���K�{�B
�����I�iblocking�j�Ȏ�M�����ƕ`�敔�ŃX���b�h�𕪂��Ă���B�O���[�o���ϐ��Ŋp�x�l�����L�B�iread/write�����m�ɕ�����Ă���̂ŁAqueue��mutex�������K�v���Ȃ��B���Ԃ�j
�`���Direct2D�B�n�[�h�R�[�h���ꂽ�f�B���N�g���̎ʐ^�����ׂēǂ݂ɍs���B

�E�肪�E�Ђ�����ɂ���������͍��肪���Ђ�����ɂ���ꍇ�A
�p�x=arctan(���y-�Ђ�y,���x-�Ђ�x)
�i�E��D��j

�p�x�����ƂɁA�ʐ^��؂�ւ��Ă����B

### �ЂƂ���
�ŏ���GPU���������g���؂�o�O���������B���܂͔�䍂��Ĕ��Ȃ����̂ŁA���̃o�O�͔������Ȃ��B�����̓��\�[�X�̉�����Ȃ���Ă��Ȃ����Ƃ������B

�ق��ɂ���肪���邩������Ȃ��B

# �J����
Visual C++
Windows 8 Pro, Visual Studio 2012 Ultimate
���؂́AWindows 8 Pro 64bit , Core i5 2520M, NVIDIA NVS 4200M + Intel HD Graphics 3000�̃}�V����Windows 7 Professional SP1 64bit, Core 2 Duo P8700 , NVIDIA GeForce 9400M�̊��ōs�����i���}�V���Ƃ�8GB RAM���ځj