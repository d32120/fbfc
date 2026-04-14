set "chars=500"
set "fname=output.bf"
set "argn=args.txt"

python fuzzy.py %chars% %fname% 0
python fuzzy.py %chars% %argn% 1
python bfinterpreter.py %fname% %argn%