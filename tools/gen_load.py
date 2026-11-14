#!/usr/bin/env python3
"""Prints a large valid script: N students (default 3000), each taking 40 courses."""
import random
import sys

students = int(sys.argv[1]) if len(sys.argv) > 1 else 3000
random.seed(1)
courses = [f"C{i:02d}" for i in range(40)]
terms = [f"{2020 + i // 3}-{i % 3 + 1}" for i in range(15)]
grades = ["A", "A-", "B", "B-", "C", "C-", "D"]
for c in courses:
    print(f"COURSE {c} REGULAR 3 CORE")
for s in range(students):
    roll = f"R{s:05d}"
    print(f"STUDENT {roll} Name{s}")
    for i, c in enumerate(courses):
        term = terms[i // 3]
        print(f"REGISTER {roll} {c} {term}")
        print(f"GRADE {roll} {c} {term} {random.choice(grades)}")
    print(f"GPA {roll}")
    print(f"AVG {roll}")
    print(f"GRAD {roll}")
    print(f"TRANSCRIPT {roll}")
