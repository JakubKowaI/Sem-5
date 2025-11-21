function mbisekcji(f, a::Float64, b::Float64, delta::Float64, epsilon::Float64)
    u=f(a)
    v=f(b)
    # if u==0.0
    #     return a,u,0,0
    # end
    # if v==0.0
    #     return a,v,0,0
    # end
    e=b-a
    if sign(u)==sign(v)
        #throw("Nie ma gwarancji zera")
        return 0,0,0,1
    end
    k=1
    local c
    local w
    for k in 1:40
        e=e/2.0
        c=a+e
        w=f(c)
        if abs(e)<delta || abs(w)<epsilon
            return c,w,k,0
        end
        if sign(u)!=sign(w)
            b=c
            v=w
        else
            a=c
            u=w
        end
    end
    return c, w, k, 1
end

function mstycznych(f,pf,x0::Float64, delta::Float64, epsilon::Float64, maxit::Int)
    v=f(x0)
    if abs(v)<epsilon
        return x0,v,0,0
    end
    k=1
    local x1
    for k in 1:maxit 
        if pf(x0)<delta
            return x0,v,k,2
        end
        x1=x0-v/pf(x0)
        v=f(x1)
        if abs(x1-x0)<delta || abs(v)<epsilon
            return x1,v,k,0
        end
        x0=x1
    end
    return x01,v,k,1
end

function msiecznych(f, x0::Float64, x1::Float64, delta::Float64, epsilon::Float64, maxit::Int)
    fa=f(x0)
    fb=f(x1)
    k=1
    for k in 1:maxit
        # println("1: ", x0)
        if abs(fa)>abs(fb)
            temp=x0
            x0=x1
            x1=temp
            temp=fa
            fa=fb
            fb=temp
        end
        s=(x1-x0)/(fb-fa)
        # println("s: ",s)
        x1=x0
        fb=fa
        x0=x0-fa*s
        # println("After: ",x0)
        fa=f(x0)
        if abs(x1-x0) < delta || abs(fa) < epsilon
            return x0,fa,k,0
        end
    end
    return x0,fa,k,1
end