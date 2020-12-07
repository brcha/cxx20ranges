evenSquares :: [Int] -> [Int]
evenSquares l = map (^2) $ filter even l

main :: IO ()
main = print $ evenSquares [1, 2, 3, 4, 5]
