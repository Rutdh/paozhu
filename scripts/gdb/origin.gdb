# 调用方法-  (gdb):  dtinfo_init "info_args"
define dtinfo_init
  delete
  if ((int)strcmp($arg0 ,"info_args") ==0)
    b -f info_args
    commands $bpnum
      printf "==> -------- info_args -----------------------\n"
      printf "==> INPUT name[%s] lnum[%d] ft[%s] \n", name, lnum, ft
      continue
    end
    # b -f searchinfo -label end if $_streq(ret,"tec")
    b -f searchinfo -label end if (!(int)strncmp(ret, "tec", strlen("tec")))
    commands $bpnum
      printf "==> -------- searchinfo -----------------------\n"
      printf "==> INPUT type[%s] infofile[%s], name_in[%s],\n", type, infofile, name_in
      printf "==> Return [%s]\n", ret 
      continue
    end
    b -f info_args -label end
    commands $bpnum
      printf "==> -------- info_args Return -----------------------\n"
      printf "%s", list_print(ret_l)
      continue
    end
    b -f info_start
    commands $bpnum
      printf "==> -------- info_start Input  -----------------------\n"
      printf "%s", list_print(args)
      continue
    end
    b -f info_curwin
    commands $bpnum
      printf "==> -------- info_curwin ------------------\n"
      printf "==> fname[%s] index[%s] node[%s] searchstr[%s]\n", fname, index, node, searchstr
      continue
    end
    b -f info_main
    commands $bpnum
      printf "==> -------- info_main ------------------\n"
      guile (plist "argv" #f  0 10)
      continue
    end
end