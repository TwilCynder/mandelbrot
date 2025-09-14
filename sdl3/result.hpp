template <typename R, typename E>
class Result {
    bool success_;

    union Content {
        R result;
        E error;
    } val;

    public:

    Result(R res): val(res), success_(true) {};
    Result(const R& res): val(res), success_(true) {};
    Result(E err): val(err), success_(false) {};
    Result(const E& err): val(err), success_(false) {};

    static Result<R, E> Success(R res){return Result(res);}
    static Result<R, E> Success(const R& res){return Result(res);}
    static Result<R, E> Error(E err){return Result(err);}
    static Result<R, E> Error(const E& err){return Result(err);}
    static Result<R, E> Error(){return Result(R{});}

    bool success() const {return success_;}
    const R& getResult() const {return val.result;}
    const E& getError() const {return val.error;}
};